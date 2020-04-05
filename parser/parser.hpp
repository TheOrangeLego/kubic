#ifndef _PARSER_HPP
#define _PARSER_HPP

#include <stack>
#include <queue>

#include "parser/rules.hpp"
#include "shared/helpers.hpp"
#include "shared/node.hpp"
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

  while ( !_tokens.empty() ) {
    headToken = _tokens.front();
    _tokens.pop();

    switch ( headToken.getType() ) {
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

Node* parseBinding( std::queue<Token>& _tokens, const Token _variable ) {
  Token typeBinding = _tokens.front();
  _tokens.pop();

  Token dataType = _tokens.front();
  _tokens.pop();

  Token bodyBinding = _tokens.front();
  _tokens.pop();

  Node* bindingExpression = parse( _tokens );
  return new BindingNode( _variable, DATA_TYPE_MAPS[dataType.getText()], bindingExpression );
}

Node* parse( std::queue<Token>& _tokens ) {
  if ( _tokens.empty() ) {
    return nullptr;
  }

  Token headToken = _tokens.front();

  switch ( headToken.getType() ) {
    case TokenType::ConstantTokenType:
    case TokenType::VariableTokenType:
    case TokenType::GroupTokenType:
      return parseArithmetic( _tokens );
      break;
    default:
      break;
  }

  return nullptr;
}

#endif
