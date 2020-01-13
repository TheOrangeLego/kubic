#ifndef _NODEIFY_HPP
#define _NODEIFY_HPP

#include <queue>
#include <stack>

#include "rules.hpp"
#include "../shared/helpers.hpp"
#include "../shared/messages.hpp"
#include "../shared/token/Token.hpp"
#include "../shared/node/Node.hpp"

Node* nodeify( std::queue<Token>& _tokens, std::stack<TokenError>& _errors );

Node* nodeifyConstant( std::stack<Token>& _tokens, std::stack<TokenError>& _errors ) {
  Token token = _tokens.top();
  _tokens.pop();

  return new ConstantNode( token );
}

Node* nodeifyVariable( std::stack<Token>& _tokens, std::stack<TokenError>& _errors ) {
  Token token = _tokens.top();
  _tokens.pop();

  return new VariableNode( token );
}

Node* nodeifyUnaryOperator( std::stack<Token>& _tokens, std::stack<TokenError>& _errors ) {
  Token token = _tokens.top();
  _tokens.pop();

  Node* node = nodeifyArithmetic( _tokens, _errors );

  return new UnaryOperatorNode( token, node );
}

Node* nodeifyBinaryOperator( std::stack<Token>& _tokens, std::stack<TokenError>& _errors ) {
  Token token = _tokens.top();
  _tokens.pop();

  Node* rNode = nodeifyArithmetic( _tokens, _errors );
  Node* lNode = nodeifyArithmetic( _tokens, _errors );

  return new BinaryOperatorNode( token, lNode, rNode );
}

Node* nodeifyArithmetic( std::stack<Token>& _tokens, std::stack<TokenError>& _errors ) {
  if ( _tokens.empty() ) {
    return nullptr;
  }

  Token token = _tokens.top();

  switch ( token.getType() ) {
    case TokenType::Constant:
      return nodeifyConstant( _tokens, _errors );
      break;
    case TokenType::Variable:
      return nodeifyVariable( _tokens, _errors );
      break;
    case TokenType::Operator:
      /* determine if unary or binary operator */
      return nodeifyBinaryOperator( _tokens, _errors );
      break;
    default:
      return nullptr;
      break;
  }
}

Node* nodeifyArithmetic( std::queue<Token>& _tokens, std::stack<TokenError>& _errors ) {
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

  return nodeifyArithmetic( organizedTokens, _errors );
}

Node* nodeifyBinding( std::queue<Token>& _tokens, std::stack<TokenError>& _errors ) {
  // Token letToken = _tokens.front();
  // _tokens.pop();

  // if ( _tokens.empty() ) {
  //   _errors.push( TokenError( letToken, ERR_MISSING_VARIABLE_BINDING ) );
  //   return nullptr;
  // }

  // Token variableToken = _tokens.front();
  // _tokens.pop();

  // if ( _tokens.empty() ) {
  //   _errors.push( TokenError( variableToken, ERR_MISSING_ASSIGNMENT_BINDING ) );
  //   return nullptr;
  // }
  
  // Token assignmentToken = _tokens.front();

  // if ( !equals( assignmentToken.getToken(), "=" ) ) {
  //   _errors.push( TokenError( assignmentToken, ERR_EXPECTED_ASSIGNMENT_BINDING ) );
  //   return nullptr;
  // }
  // _tokens.pop();

  // Token currentToken = _tokens.front();
  // std::queue<Token> bindingTokens;

  // while ( !_tokens.empty() && !equals( currentToken.getToken(), "::" ) ) {
  //   bindingTokens.push( currentToken );
  //   _tokens.pop();
  //   currentToken = _tokens.front();
  // }
  
  // if ( _tokens.empty() ) {
  //   _errors.push( TokenError( currentToken, ERR_MISSING_ENDING_BINDING ) );
  //   return nullptr;
  // }

  // if ( !equals( currentToken.getToken(), "::" ) ) {
  //   _errors.push( TokenError( currentToken, ERR_EXPECTED_ENDING_BINDING ) );
  //   return nullptr;
  // }
  // _tokens.pop();

  // return new BindingNode( variableToken, treeify( bindingTokens, _errors ), treeify( _tokens, _errors ) );
}

Node* nodeify( std::queue<Token>& _tokens, std::stack<TokenError>& _errors ) {
  if ( _tokens.empty() ) {
    return nullptr;
  }

  Token currentToken = _tokens.front();

  switch ( currentToken.getType() ) {
    case TokenType::Constant:
    case TokenType::Variable:
      return nodeifyArithmetic( _tokens, _errors );
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
