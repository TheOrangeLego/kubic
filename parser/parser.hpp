#ifndef _PARSER_HPP
#define _PARSER_HPP

#include <stack>
#include <queue>

#include "../shared/helpers.hpp"
#include "../shared/node.hpp"
#include "rules.hpp"
#include "../shared/token.hpp"
#include "../shared/types.hpp"

/* forward declaration */
Node* parse( std::queue<Token>& _tokens );

/* forward declaration */
Node* parseArithmetic( std::stack<Token>& _tokens );

Node* parseConstant( std::stack<Token>& _tokens ) {
  Token constant = _tokens.top();
  _tokens.pop();

  return new ConstantNode( constant );
}

Node* parseVariable( std::stack<Token>& _tokens ) {
  Token variable = _tokens.top();
  _tokens.pop();

  Token nextToken = _tokens.top();

  if ( equals( nextToken.getText(), "::" ) ) {
    return parseBinding( _tokens, variable );
  }

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
    case TokenType::ConstantToken:
      return parseConstant( _tokens );
      break;
    case TokenType::VariableToken:
      return parseVariable( _tokens );
      break;
    case TokenType::OperatorToken:
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
  Token headToken = _tokens.front();
  Token headOperator( "" );

  _tokens.pop();

  if ( equals( _tokens.front, "::" ) ) {
    return parseBinding( _tokens, headToken );
  }

  while ( !_tokens.empty() && (
    headToken.getType() == TokenType::ConstantToken ||
    headToken.getType() == TokenType::VariableToken ||
    headToken.getType() == TokenType::OperatorToken ||
    headToken.getType() == TokenType::GroupToken
  )) {
    switch ( headToken.getType() ) {
      case TokenType::ConstantToken:
      case TokenType::VariableToken:
        organizedTokens.push( headToken );
        break;
      case TokenType::GroupToken:
        if ( equals( headToken.getText(), "(" ) ) {
          operatorTokens.push( headToken );
        } else if ( equals( headToken.getText(), ")" ) ) {
          headOperator = operatorTokens.top();
          operatorTokens.pop();

          while ( !equals( headOperator.getText(), "(" ) ) {
            organizedTokens.push( headOperator );
            headOperator = operatorTokens.top();
            operatorTokens.pop();
          }
        }
        break;
      case TokenType::OperatorToken:
        while ( !operatorTokens.empty() && !hasHigherPriority( headToken, operatorTokens.top() ) ) {
          organizedTokens.push( operatorTokens.top() );
          operatorTokens.pop();
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
  Token typeBinding = _tokens.top();
  _tokens.pop();

  Token dataType = _tokens.top();
  _tokens.pop();

  Token bodyBinding = _tokens.top();
  _tokens.pop();

  Node* bindingExpression = parse( _tokens );
  return BindingNode( _variable, DATA_TYPE_MAPS[dataType.getText()], bindingExpression );
}

Node* parse( std::queue<Token>& _tokens ) {
  if ( _tokens.empty() ) {
    return nullptr;
  }

  Token headToken = _tokens.front();

  switch ( headToken.getType() ) {
    case TokenType::ConstantToken:
    case TokenType::VariableToken:
      return parseArithmetic( _tokens );
      break;
    default:
      break;
  }
}

#endif
