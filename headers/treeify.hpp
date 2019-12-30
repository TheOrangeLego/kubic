#ifndef _TREEIFY_HPP
#define _TREEIFY_HPP

#include <queue>
#include <stack>
#include <string>

#include "rules.hpp"
#include "token/Token.hpp"
#include "tree/Tree.hpp"

static bool hasHigherPriority( const Token _tokenA, const Token _tokenB ) {
  return OPERATOR_PRIORITIES[_tokenA.getToken()]> OPERATOR_PRIORITIES[_tokenB.getToken()];
}

static std::stack<Token> organizeArithmetic( std::queue<Token>& _tokens ) {
  std::stack<Token> outputStack;
  std::stack<Token> operatorStack;
  Token currentToken = _tokens.front();
  Token currentOperator( "" );

  while ( !_tokens.empty() && (
          currentToken.getType() == TokenType::Constant ||
          currentToken.getType() == TokenType::Variable ||
          currentToken.getType() == TokenType::LeftGroup  ||
          currentToken.getType() == TokenType::RightGroup ||
          currentToken.getType() == TokenType::Operator ) ) {
    
    switch ( currentToken.getType() ) {
      case TokenType::Constant:
      case TokenType::Variable:
        outputStack.push( currentToken );
        break;
      case TokenType::LeftGroup:
        operatorStack.push( currentToken );
        break;
      case TokenType::RightGroup:
        currentOperator = operatorStack.top();
        operatorStack.pop();

        while ( currentOperator.getType() != TokenType::LeftGroup ) {
          outputStack.push( currentOperator );
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
    currentToken = _tokens.front();
  }

  while ( !operatorStack.empty() ) {
    outputStack.push( operatorStack.top() );
    operatorStack.pop();
  }

  return outputStack;
}

static TreeNode* treeifyArithmetic( std::stack<Token>& _tokens ) {
  if ( _tokens.empty() ) {
    return nullptr;
  }

  Token currentToken = _tokens.top();

  if ( currentToken.getType() == TokenType::Constant ) {
    _tokens.pop();
    return new ConstNode( currentToken );
  } else if ( currentToken.getType() == TokenType::Variable ) {
    return nullptr;
  } else if ( currentToken.getType() == TokenType::Operator ) {
    _tokens.pop();

    /* as operands are retrieved backwards when parsed, place them back in order when treeifying */
    TreeNode* rightNode = treeifyArithmetic( _tokens );
    TreeNode* leftNode  = treeifyArithmetic( _tokens );
    return new BinaryOperator( currentToken, leftNode, rightNode );
  } else {
    return nullptr;
  }
}

static TreeNode* treeifyArithmetic( std::queue<Token>& _tokens ) {
  std::stack<Token> organizedStack = organizeArithmetic( _tokens );

  return treeifyArithmetic( organizedStack );
}

TreeNode* treeify( std::queue<Token>& _tokens ) {
  TreeNode* node = nullptr;

  if ( !_tokens.empty() ) {
    Token currentToken = _tokens.front();

    switch ( currentToken.getType() ) {
      case TokenType::Constant:
      case TokenType::Variable:
      case TokenType::LeftGroup: // TODO - this should check whether the next token is a constant / variable -> arithmetic; o/w recurse
        node = treeifyArithmetic( _tokens );
        break;
      case TokenType::Keyword:
        
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
