#ifndef _TREEIFY_HPP
#define _TREEIFY_HPP

#include <map>
#include <queue>
#include <stack>
#include <string>
#include "../token/Token.hpp"
#include "../tree/Tree.hpp"

static std::map<std::string, int> operatorPriorities = {
  {"+", 2}, {"-", 2},
  {"*", 3}, {"/", 3}
};

static bool hasHigherPriority( const Token _tokenA, const Token _tokenB ) {
  return operatorPriorities[_tokenA.getToken()]> operatorPriorities[_tokenB.getToken()];
}

static std::stack<Token> organizeArithmetic( std::queue<Token>& _tokens ) {
  std::stack<Token> outputStack;
  std::stack<Token> operatorStack;
  Token currentToken = _tokens.front();

  while ( !_tokens.empty() && (
          currentToken.getType() == TokenType::Constant ||
          currentToken.getType() == TokenType::Variable ||
          currentToken.getType() == TokenType::Operator ) ) {
    
    switch ( currentToken.getType() ) {
      case TokenType::Constant:
      case TokenType::Variable:
        outputStack.push( currentToken );
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
    return new BinaryOperator( currentToken, treeifyArithmetic( _tokens ), treeifyArithmetic( _tokens ) );
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
        node = treeifyArithmetic( _tokens );
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
