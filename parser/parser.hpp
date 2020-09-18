#ifndef _PARSER_HPP
#define _PARSER_HPP

#include <map>
#include <queue>
#include <stack>

#include "parser/lexer.hpp"
#include "parser/node.hpp"
#include "parser/token.hpp"

#include "shared/types.hpp"

const std::map<std::string, unsigned int> OPERATOR_PRIORITY = {
  { "(", 0 }, { ")", 0 },
  { "+", 1 }, { "-", 1 },
  { "*", 2 }, { "/", 2 },
};

bool higherPriority( const Token* _tokenA, const Token* _tokenB ) {
  return OPERATOR_PRIORITY.at( _tokenA->getText() ) >= OPERATOR_PRIORITY.at( _tokenB->getText() );
}

Node* nodeify( std::queue<Token*>& _tokens );

Node* nodeifyStatements( std::queue<Token*>& _tokens ) {
  std::vector<Node*> statements;

  while ( !_tokens.empty() && _tokens.front()->getText() != "}" ) {
    Node* statement = nodeify( _tokens );

    if ( statement ) {
      statements.push_back( statement );
    }
  }

  if ( statements.empty() ) {
    return new MultiStatementNode( Position( 0, 0, "" ), statements );
  } else {
    return new MultiStatementNode( statements[0]->getPosition(), statements );
  }
}

Node* nodeifyGroupedStatements( std::queue<Token*>& _tokens ) {
  Token* openGroup = _tokens.front();
  _tokens.pop();

  while ( !_tokens.empty() && openGroup->getText() != "{" ) {
    openGroup = _tokens.front();
    _tokens.pop();
  }

  Node* statements = nodeifyStatements( _tokens );

  Token* closeGroup = _tokens.front();
  _tokens.pop();

  while ( !_tokens.empty() && closeGroup->getText() == "\n" ) {
    closeGroup = _tokens.front();
    _tokens.pop();
  }

  return statements;
}

Node* nodeifyArithmetic( std::stack<Token*>& _tokens ) {
  if ( _tokens.empty() ) {
    return nullptr;
  }

  Node* node = nullptr;
  Token* top = _tokens.top();
  _tokens.pop();

  switch ( top->getType() ) {
    case TokenType::TokenBoolean:
      node = new BooleanNode( top->getText(), top->getPosition() );
      break;
    case TokenType::TokenConstant:
      node = new ConstantNode( top->getText(), top->getPosition() );
      break;
    case TokenType::TokenVariable:
      node = new VariableNode( top->getText(), top->getPosition() );
      break;
    case TokenType::TokenOperator: {
      Node* lNode = nodeifyArithmetic( _tokens );
      Node* rNode = nodeifyArithmetic( _tokens );
      node = new BinaryOperatorNode( top->getText(), top->getPosition(), lNode, rNode );
      break;
    }
    default:
      break;
  }

  return node;
}

Node* nodeifyArithmetic( std::queue<Token*>& _tokens ) {
  std::stack<Token*> operatorTokens;
  std::stack<Token*> organizedTokens;

  Token* topToken = nullptr;
  Token* headToken = nullptr;

  while ( !_tokens.empty() && contains( ARITHMETIC_TYPES, _tokens.front()->getType() ) ) {
    headToken = _tokens.front();

    switch ( headToken->getType() ) {
      case TokenType::TokenBoolean:
      case TokenType::TokenConstant:
      case TokenType::TokenVariable:
        organizedTokens.push( headToken );
        break;
      case TokenType::TokenOperator:
        if ( !operatorTokens.empty() ) {
          topToken = operatorTokens.top();

          while ( topToken && !higherPriority( headToken, topToken ) ) {
            organizedTokens.push( topToken );
            operatorTokens.pop();
            topToken = operatorTokens.empty() ? nullptr : operatorTokens.top();
          }

          operatorTokens.push( headToken );
        } else {
          operatorTokens.push( headToken );
        }
        break;
      case TokenType::TokenArithmeticGrouper:
        if ( headToken->getText() == "(" ) {
          operatorTokens.push( headToken );
        } else {
          topToken = operatorTokens.top();

          while ( topToken && topToken->getText() != "(" ) {
            organizedTokens.push( topToken );
            operatorTokens.pop();
            topToken = operatorTokens.empty() ? nullptr : operatorTokens.top();
          }

          operatorTokens.pop();
        }
        break;
      default:
        break;
    }

    _tokens.pop();
  }

  while ( !operatorTokens.empty() ) {
    organizedTokens.push( operatorTokens.top() );
    operatorTokens.pop();
  }

  return nodeifyArithmetic( organizedTokens );
}

Node* nodeifyBinding( std::queue<Token*>& _tokens ) {
  Node* node = nullptr;

  Token* variable = _tokens.front();
  _tokens.pop();

  Token* typeDefine = _tokens.front();
  _tokens.pop();

  Token* type = _tokens.front();
  _tokens.pop();

  Token* assignOperator = _tokens.front();
  _tokens.pop();

  Node* bindingExpression = nodeify( _tokens );

  if ( translateToValueType( type->getText() ) != bindingExpression->getValueType() ) {
    log(
      Severity::Error,
      type->getPosition(),
      ERR_BINDING_TYPE_MISMATCH,
      bindingExpression->getValueType(),
      type->getText()
    );
  }

  addVariable( variable->getText(), bindingExpression->getValueType() );
  node = new BindingNode( variable->getText(), variable->getPosition(), bindingExpression );

  return node;
}

Node* nodeifyIfElse( std::queue<Token*>& _tokens, Token* _token ) {
  Node* node = nullptr;

  Node* conditional = nodeify( _tokens );

  Node* upperBody = nodeifyGroupedStatements( _tokens );

  Node* lowerBody = nullptr;

  if ( !_tokens.empty() && _tokens.front()->getText() == "elif" ) {
    Token* headToken = _tokens.front();
    _tokens.pop();

    lowerBody = nodeifyIfElse( _tokens, headToken );
  } else if ( !_tokens.empty() && _tokens.front()->getText() == "else" ) {
    _tokens.pop();

    lowerBody = nodeifyGroupedStatements( _tokens );
  }

  node = new ConditionalNode( _token->getPosition(), conditional, upperBody, lowerBody );

  return node;
}

Node* nodeifyKeyword( std::queue<Token*>& _tokens ) {
  Node* node = nullptr;

  Token* token = _tokens.front();
  _tokens.pop();

  if ( token->getText() == "define" ) {
    node = nodeifyBinding( _tokens );
  } else if ( token->getText() == "if" ) {
    node = nodeifyIfElse( _tokens, token );
  }

  return node;
}

Node* nodeify( std::queue<Token*>& _tokens ) {
  if ( _tokens.empty() ) {
    return nullptr;
  }

  Token* head = _tokens.front();

  Node* node = nullptr;

  switch ( head->getType() ) {
    case TokenType::TokenNewline:
      _tokens.pop();
      node = nodeify( _tokens );
      break;
    case TokenType::TokenBoolean:
    case TokenType::TokenConstant:
    case TokenType::TokenVariable:
    case TokenType::TokenArithmeticGrouper:
      node = nodeifyArithmetic( _tokens );
      break;
    case TokenType::TokenKeyword:
      node = nodeifyKeyword( _tokens );
      break;
    default:
      break;
  }

  return node;
}

Node* parse( const std::string _filename ) {
  std::queue<Token*> tokens = tokenize( _filename );

  if ( !emptyErrorsLog() ) {
    return nullptr;
  } else {
    return nodeifyStatements( tokens );
  }
}

#endif
