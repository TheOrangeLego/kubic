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
  { "<", 9 }, { ">", 9 }, { ">=", 9 }, { "<=", 9 },
};

bool higherPriority( const Token* _tokenA, const Token* _tokenB ) {
  return OPERATOR_PRIORITY.at( _tokenA->getText() ) >= OPERATOR_PRIORITY.at( _tokenB->getText() );
}

Node* nodeify( std::queue<Token*>& _tokens );

std::vector<Node*> nodeifyCommaSeparatedExpressions( std::queue<Token*>& _tokens ) {
  std::vector<Node*> expressions;
  bool moreExpressions = true;

  do {
    expressions.push_back( nodeify( _tokens ) );

    if ( _tokens.empty() || _tokens.front()->getText() != "," ) {
      moreExpressions = false;
    } else {
      _tokens.pop();

      while ( _tokens.front()->getText() == "\n" ) {
        _tokens.pop();
      }
    }
  } while ( moreExpressions );

  return expressions;
}

Node* nodeifyFunctionCall( std::queue<Token*>& _tokens, Token* _token ) {
  Node* node = nullptr;
  std::vector<Node*> arguments;

  Token* openArgument = _tokens.front();
  _tokens.pop();

  int innerExpressionCount = 0;

  std::queue<Token*> argumentTokens;

  while ( !_tokens.empty() && ( _tokens.front()->getText() != ")" || innerExpressionCount ) ) {
    if ( _tokens.front()->getText() == "(" ) {
      innerExpressionCount++;
    } else if ( _tokens.front()->getText() == ")" ) {
      innerExpressionCount--;
    }

    argumentTokens.push( _tokens.front() );
    _tokens.pop();
  }

  arguments = nodeifyCommaSeparatedExpressions( argumentTokens );

  Token* closeArgument = _tokens.front();
  _tokens.pop();

  node = new FunctionCallNode( _token->getPosition(), _token->getText(), arguments );

  return node;
}

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

Node* nodeifyArithmetic( std::queue<Token*>& _tokens ) {
  bool continueParsing = true;
  std::stack<Node*> operands;
  std::stack<Token*> operators;

  Token* top = _tokens.front();

  while ( continueParsing && top ) {
    if ( top->getType() == TokenType::TokenBoolean ) {
      operands.push( new BooleanNode( top->getText(), top->getPosition() ) );
      _tokens.pop();
    } else if ( top->getType() == TokenType::TokenConstant ) {
      operands.push( new ConstantNode( top->getText(), top->getPosition() ) );
      _tokens.pop();
    } else if ( top->getType() == TokenType::TokenVariable ) {
      Token* variable = top;
      _tokens.pop();

      if ( contains( functions, variable->getText() ) &&  _tokens.front()->getText() == "(" ) {
        operands.push( nodeifyFunctionCall( _tokens, variable ) );
      } else {
        operands.push( new VariableNode( variable->getText(), variable->getPosition() ) );
        _tokens.pop();
      }
    } else if ( top->getType() == TokenType::TokenOperator ) {
      if ( !operators.empty() ) {
        Token* topOperator = operators.top();

        while ( topOperator && !higherPriority( top, topOperator ) ) {
          Node* rOperand = operands.top();
          operands.pop();
          Node* lOperand = operands.top();
          operands.pop();
          Node* newOperand = new BinaryOperatorNode(
            topOperator->getText(), topOperator->getPosition(), lOperand, rOperand
          );
          operands.push( newOperand );
          operators.pop();
          topOperator = operators.empty() ? nullptr : operators.top();
        }
      }

      operators.push( top );
      _tokens.pop();
    } else if ( top->getType() == TokenType::TokenArithmeticGrouper ) {
      if ( top->getText() == "(" ) {
        operators.push( top );
      } else {
        Token* topOperator = operators.top();

        while ( topOperator && topOperator->getText() != "(" ) {
          Node* rOperand = operands.top();
          operands.pop();
          Node* lOperand = operands.top();
          operands.pop();
          Node* newOperand = new BinaryOperatorNode(
            topOperator->getText(), topOperator->getPosition(), lOperand, rOperand
          );
          operands.push( newOperand );
          operators.pop();
          topOperator = operators.empty() ? nullptr : operators.top();
        }

        operators.pop();
      }

      _tokens.pop();
    } else {
      continueParsing = false;
    }

    top = _tokens.empty() ? nullptr : _tokens.front();
  }

  while ( !operators.empty() ) {
    Token* topOperator = operators.top();
    operators.pop();

    Node* rOperand = operands.top();
    operands.pop();
    Node* lOperand = operands.top();
    operands.pop();
    Node* newOperand = new BinaryOperatorNode( topOperator->getText(), topOperator->getPosition(), lOperand, rOperand );
    operands.push( newOperand );
  }

  Node* topOperand = operands.top();
  operands.pop();

  if ( !operands.empty() ) {
    /* TODO -- error :: should not have remaining nodes */
  }

  return topOperand;
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
