#ifndef _PARSER_HPP
#define _PARSER_HPP

#include <queue>
#include <stack>

#include "shared/helpers.hpp"
#include "shared/node.hpp"
#include "parser/rules.hpp"
#include "shared/token.hpp"
#include "shared/types.hpp"

/* forward declaration */
Node* parse( std::queue<Token>& _tokens );

/* forward declaration */
Node* parseArithmetic( std::stack<Token>& _tokens );

/* forward declaration */
Node* parseBinding( std::queue<Token>& _tokens, const Token _variable );

Node* parseConstant( std::stack<Token>& _tokens ) {
  Token constant = _tokens.top();
  _tokens.pop();

  return new ConstantNode( constant );
}

Node* parseVariable( std::stack<Token>& _tokens ) {
  Token variable = _tokens.top();
  _tokens.pop();

  return new VariableNode( variable );
}

Node* parseUnaryOperator( std::stack<Token> & _tokens ) {
  Token uOperator = _tokens.top();
  _tokens.pop();

  Node* operand = parseArithmetic( _tokens );

  return new UnaryOperatorNode( uOperator, operand );
}

Node* parseBinaryOperator( std::stack<Token>& _tokens ) {
  Token bOperator = _tokens.top();
  _tokens.pop();

  Node* rOperand = parseArithmetic( _tokens );
  Node* lOperand = parseArithmetic( _tokens );

  return new BinaryOperatorNode( bOperator, lOperand, rOperand );
}

Node* parseArithmetic( std::stack<Token>& _tokens ) {
  if ( _tokens.empty() ) {
    return nullptr;
  }

  Token token = _tokens.top();

  switch ( token.getType() ) {
    case TokenType::ConstantTokenType:
      return parseConstant( _tokens );
      break;
    case TokenType::VariableTokenType:
      return parseVariable( _tokens );
      break;
    case TokenType::OperatorTokenType:
      if ( contains( UNARY_OPERATORS, token.getText() ) ) {
        return parseUnaryOperator( _tokens );
      }

      return parseBinaryOperator( _tokens );
      break;
    default:
      return nullptr;
      break;
  }
}

Node* parseArithmetic( std::queue<Token>& _tokens ) {
  std::stack<Token> operatorTokens;
  std::stack<Token> organizedTokens;
  Token headToken;
  Token topOperatorToken;
  bool stopParsing = false;

  while ( !stopParsing && !_tokens.empty() ) {
    headToken = _tokens.front();
    _tokens.pop();

    switch ( headToken.getType() ) {
      case TokenType::NewlineTokenType:
        stopParsing = true;
        break;
      case TokenType::ConstantTokenType:
      case TokenType::VariableTokenType:
        organizedTokens.push( headToken );
        break;
      case TokenType::GroupTokenType:
        if ( equals( headToken, "(" ) ) {
          operatorTokens.push( headToken );
        } else {
          topOperatorToken = operatorTokens.top();

          while ( !equals( topOperatorToken, "(" ) ) {
            organizedTokens.push( topOperatorToken );
            operatorTokens.pop();
            topOperatorToken = operatorTokens.top();
          }

          operatorTokens.pop();
        }
        break;
      case TokenType::OperatorTokenType:
        if ( !operatorTokens.empty() ) {
          topOperatorToken = operatorTokens.top();
        }

        while ( !operatorTokens.empty() && !hasHigherPriority( headToken, topOperatorToken ) ) {
          organizedTokens.push( topOperatorToken );
          operatorTokens.pop();
          topOperatorToken = operatorTokens.top();
        }

        operatorTokens.push( headToken );
        break;
      default:
        break;
    }
  }

  while ( !operatorTokens.empty() ) {
    organizedTokens.push( operatorTokens.top() );
    operatorTokens.pop();
  }

  return parseArithmetic( organizedTokens );
}

Node* parseBinding( std::queue<Token>& _tokens ) {
  Token bindKeyword = _tokens.front();
  _tokens.pop();

  Token variable = _tokens.front();
  _tokens.pop();

  Token typeBinder = _tokens.front();
  _tokens.pop();

  Token typeBinded = _tokens.front();
  _tokens.pop();

  Token exprBinder = _tokens.front();
  _tokens.pop();

  Node* bindingExpression = parse( _tokens );
  return new BindingNode( variable, DATA_TYPE_MAPS[typeBinded.getText()], bindingExpression );
}

Node* parse( std::queue<Token>& _tokens ) {
  if ( _tokens.empty() ) {
    return nullptr;
  }

  Token headToken = _tokens.front();

  switch ( headToken.getType() ) {
    case TokenType::NewlineTokenType:
      _tokens.pop();
      return parse( _tokens );
      break;
    case TokenType::ConstantTokenType:
    case TokenType::VariableTokenType:
    case TokenType::GroupTokenType:
      return parseArithmetic( _tokens );
      break;
    case TokenType::KeywordTokenType:
      if ( equals( headToken, "let" ) ) {
        return parseBinding( _tokens );
        break;
      }
    default:
      break;
  }

  return nullptr;
}

#endif
