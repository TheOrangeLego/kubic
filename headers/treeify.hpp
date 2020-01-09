#ifndef _TREEIFY_HPP
#define _TREEIFY_HPP

#include <queue>
#include <stack>
#include <string>

#include "rules.hpp"
#include "messages.hpp"
#include "token/Token.hpp"
#include "tree/Tree.hpp"

TreeNode* treeify( std::queue<Token>& _tokens, std::stack<TokenError>& _errors );

static bool hasHigherPriority( const Token _tokenA, const Token _tokenB ) {
  return OPERATOR_PRIORITIES[_tokenA.getToken()]> OPERATOR_PRIORITIES[_tokenB.getToken()];
}

static std::stack<Token> organizeArithmetic( std::queue<Token>& _tokens, std::stack<TokenError>& _errors ) {
  std::stack<Token> outputStack;
  std::stack<Token> operatorStack;
  Token currentToken = _tokens.front();
  Token currentOperator( "" );

  while ( !_tokens.empty() && ( currentToken.getType() == TokenType::Operator ||
    currentToken.getType() == TokenType::Constant || currentToken.getType() == TokenType::Variable ||
    currentToken.getType() == TokenType::LeftParenthesis  || currentToken.getType() == TokenType::RightParenthesis ) ) {

    switch ( currentToken.getType() ) {
      case TokenType::Constant:
      case TokenType::Variable:
        outputStack.push( currentToken );
        break;
      case TokenType::LeftParenthesis:
        operatorStack.push( currentToken );
        break;
      case TokenType::RightParenthesis:
        currentOperator = operatorStack.top();
        operatorStack.pop();

        while ( currentOperator.getType() != TokenType::LeftParenthesis ) {
          outputStack.push( currentOperator );

          if ( operatorStack.empty() ) {
            _errors.push( TokenError( currentToken, ERR_UNEXPECTED_EXPR_END ) );
            break;
          }

          currentOperator = operatorStack.top();
          operatorStack.pop();
        }
        break;
      case TokenType::Operator:
        while ( !operatorStack.empty() && !hasHigherPriority( currentToken, operatorStack.top() ) ) {
          outputStack.push( operatorStack.top() );
          operatorStack.pop();
        }

        operatorStack.push( currentToken );
      default:
        break;
    }
    _tokens.pop();

    /* this will quiet down Valgrind */
    if ( !_tokens.empty() ) currentToken = _tokens.front();
  }

  while ( !operatorStack.empty() ) {
    outputStack.push( operatorStack.top() );
    operatorStack.pop();
  }

  return outputStack;
}

static TreeNode* treeifyArithmetic( std::stack<Token>& _tokens, std::stack<TokenError>& _errors ) {
  if ( _tokens.empty() ) {
    return nullptr;
  }

  Token currentToken = _tokens.top();

  if ( currentToken.getType() == TokenType::Constant ) {
    _tokens.pop();
    return new ConstantNode( currentToken );
  } else if ( currentToken.getType() == TokenType::Variable ) {
    _tokens.pop();
    return new VariableNode( currentToken );
  } else if ( currentToken.getType() == TokenType::Operator ) {
    _tokens.pop();

    /* as operands are retrieved backwards when parsed, place them back in order when treeifying */
    TreeNode* rightNode = treeifyArithmetic( _tokens, _errors );
    TreeNode* leftNode  = treeifyArithmetic( _tokens, _errors );

    if ( !rightNode ) _errors.push( TokenError( currentToken, ERR_MISSING_LEFT_OPERAND ) );
    if ( !leftNode ) _errors.push( TokenError( currentToken, ERR_MISSING_RIGHT_OPERAND ) );

    return new BinaryOperator( currentToken, leftNode, rightNode );
  } else {
    return nullptr;
  }
}

static TreeNode* treeifyArithmetic( std::queue<Token>& _tokens, std::stack<TokenError>& _errors ) {
  std::stack<Token> organizedStack = organizeArithmetic( _tokens, _errors );

  return treeifyArithmetic( organizedStack, _errors );
}

static TreeNode* treeifyBinding( std::queue<Token>& _tokens, std::stack<TokenError>& _errors ) {
  Token letToken = _tokens.front();
  _tokens.pop();

  if ( _tokens.empty() ) {
    _errors.push( TokenError( letToken, ERR_MISSING_VARIABLE_BINDING ) );
    return nullptr;
  }

  Token variableToken = _tokens.front();
  _tokens.pop();

  if ( _tokens.empty() ) {
    _errors.push( TokenError( variableToken, ERR_MISSING_ASSIGNMENT_BINDING ) );
    return nullptr;
  }
  
  Token assignmentToken = _tokens.front();

  if ( !equals( assignmentToken.getToken(), "=" ) ) {
    _errors.push( TokenError( assignmentToken, ERR_EXPECTED_ASSIGNMENT_BINDING ) );
    return nullptr;
  }
  _tokens.pop();

  Token currentToken = _tokens.front();
  std::queue<Token> bindingTokens;

  while ( !_tokens.empty() && !equals( currentToken.getToken(), "::" ) ) {
    bindingTokens.push( currentToken );
    _tokens.pop();
    currentToken = _tokens.front();
  }
  
  if ( _tokens.empty() ) {
    _errors.push( TokenError( currentToken, ERR_MISSING_ENDING_BINDING ) );
    return nullptr;
  }

  if ( !equals( currentToken.getToken(), "::" ) ) {
    _errors.push( TokenError( currentToken, ERR_EXPECTED_ENDING_BINDING ) );
    return nullptr;
  }
  _tokens.pop();

  return new BindingNode( variableToken, treeify( bindingTokens, _errors ), treeify( _tokens, _errors ) );
}

TreeNode* treeify( std::queue<Token>& _tokens, std::stack<TokenError>& _errors ) {
  TreeNode* node = nullptr;

  if ( !_tokens.empty() ) {
    Token currentToken = _tokens.front();

    switch ( currentToken.getType() ) {
      case TokenType::Constant:
      case TokenType::Variable:
      case TokenType::LeftParenthesis:
        node = treeifyArithmetic( _tokens, _errors );
        break;
      case TokenType::Keyword:
        node = treeifyBinding( _tokens, _errors );
        break;
      case TokenType::Undefined:
        break;
      default:
        break;
    }
  }

  return node;
}

#endif
