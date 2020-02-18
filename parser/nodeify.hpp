#ifndef _NODEIFY_HPP
#define _NODEIFY_HPP

#include <queue>
#include <stack>

#include "rules.hpp"
#include "../shared/helpers.hpp"
#include "../shared/messages.hpp"
#include "../shared/token/Token.hpp"
#include "../shared/node/Node.hpp"

static std::stack<ParserError> parserErrors;

bool hasParserErrors() {
  return !parserErrors.empty();
}

void printParserErrors() {
}

Node* nodeify( std::queue<Token>& );
Node* nodeifyArithmetic( std::stack<Token>& );

Node* nodeifyConstant( std::stack<Token>& _tokens ) {
  Token token = _tokens.top();
  _tokens.pop();

  return new ConstantNode( token );
}

Node* nodeifyVariable( std::stack<Token>& _tokens ) {
  Token token = _tokens.top();
  _tokens.pop();

  return new VariableNode( token );
}

Node* nodeifyUnaryOperator( std::stack<Token>& _tokens ) {
  Token token = _tokens.top();
  _tokens.pop();

  Node* node = nodeifyArithmetic( _tokens );

  return new UnaryOperatorNode( token, node );
}

Node* nodeifyBinaryOperator( std::stack<Token>& _tokens ) {
  Token token = _tokens.top();
  _tokens.pop();

  Node* rNode = nodeifyArithmetic( _tokens );
  Node* lNode = nodeifyArithmetic( _tokens );

  return new BinaryOperatorNode( token, lNode, rNode );
}

Node* nodeifyArithmetic( std::stack<Token>& _tokens ) {
  if ( _tokens.empty() ) {
    return nullptr;
  }

  Token token = _tokens.top();

  switch ( token.getType() ) {
    case TokenType::Constant:
      return nodeifyConstant( _tokens );
      break;
    case TokenType::Variable:
      return nodeifyVariable( _tokens );
      break;
    case TokenType::Operator:
      /* determine if unary or binary operator */
      return nodeifyBinaryOperator( _tokens );
      break;
    default:
      return nullptr;
      break;
  }
}

Node* nodeifyArithmetic( std::queue<Token>& _tokens ) {
  std::stack<Token> operatorTokens;
  std::stack<Token> organizedTokens;
  Token currentToken = _tokens.front();
  Token currentOperator( "" );

  while ( !_tokens.empty() && (
    currentToken.getType() == TokenType::Constant || currentToken.getType() == TokenType::Variable ||
    currentToken.getType() == TokenType::Operator || currentToken.getType() == TokenType::Grouper
  ) ) {
    switch ( currentToken.getType() ) {
      case TokenType::Constant:
      case TokenType::Variable:
        organizedTokens.push( currentToken );
        break;
      case TokenType::Grouper:
        if ( equals( currentToken.getToken(), "(" ) ) {
          operatorTokens.push( currentToken );
        } else if ( equals( currentToken.getToken(), ")" ) ) {
          currentOperator = operatorTokens.top();
          operatorTokens.pop();

          while ( !equals( currentOperator.getToken(), "(" ) ) {
            organizedTokens.push( currentOperator );

            currentOperator = operatorTokens.top();
            operatorTokens.pop();
          }
        }
        break;
      case TokenType::Operator:
        while ( !operatorTokens.empty() && !hasHigherPriority( currentToken, operatorTokens.top() ) ) {
          organizedTokens.push( operatorTokens.top() );
          operatorTokens.pop();
        }

        operatorTokens.push( currentToken );
        break;
      default:
        break;
    }

    _tokens.pop();

    if ( !_tokens.empty() ) currentToken = _tokens.front();
  }

  while ( !operatorTokens.empty() ) {
    organizedTokens.push( operatorTokens.top() );
    operatorTokens.pop();
  }

  return nodeifyArithmetic( organizedTokens );
}

Node* nodeifyBinding( std::queue<Token>& _tokens ) {
  Token letToken = _tokens.front();
  _tokens.pop();
  
  /*
   * TODO -- allow the binding of multiple variables within the same let declaration,
   *         such that the following statement is considered valid
   *            let x = 3, y = 5, z = x + y :: ...
   *         This should invoke a new nodeify function that wraps multiple bindings
   *         and have its compile method simply delegate to its bindings' compile method
   */
  Token variableToken = _tokens.front();
  _tokens.pop();

  Token assignerToken = _tokens.front();
  _tokens.pop();

  Node* bindingExpression = nodeify( _tokens );

  Token terminatorToken = _tokens.front();
  _tokens.pop();

  Node* bodyExpression = nodeify( _tokens );

  return new BindingNode( variableToken, bindingExpression, bodyExpression );
}

Node* nodeify( std::queue<Token>& _tokens ) {
  if ( _tokens.empty() ) {
    return nullptr;
  }

  Token currentToken = _tokens.front();

  switch ( currentToken.getType() ) {
    case TokenType::Constant:
    case TokenType::Variable:
      return nodeifyArithmetic( _tokens );
      break;
    case TokenType::Grouper:
      break;
    case TokenType::Keyword:
      break;
    case TokenType::Undefined:
      break;
    default:
      break;
  }

}

#endif
