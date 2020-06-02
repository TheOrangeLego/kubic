#ifndef _PARSER_HPP
#define _PARSER_HPP

#include <queue>
#include <stack>

#include "shared/error.hpp"
#include "shared/helpers.hpp"
#include "shared/node.hpp"
#include "parser/rules.hpp"
#include "shared/token.hpp"
#include "shared/types.hpp"

/* helper functions */
void emptyTokens( const std::stack<Token>& _tokens, const Token _lastToken, ErrorLogger& _errorLogger ) {
  if ( _tokens.empty() ) {
    _errorLogger.logError( ERR_UNEXPECTED_TERMINATION, _lastToken );
  }
}

void emptyTokens( const std::queue<Token>& _tokens, const Token _lastToken, ErrorLogger& _errorLogger ) {
  if ( _tokens.empty() ) {
    _errorLogger.logError( ERR_UNEXPECTED_TERMINATION, _lastToken );
  }
}

void unexpectedToken( const Token _token, ErrorLogger& _errorLogger ) {
  _errorLogger.logError( ERR_UNEXPECTED_TOKEN, _token );
}

void unexpectedToken( const Token _token, const std::string _expectedToken, ErrorLogger& _errorLogger ) {
  if ( !equals( _token, _expectedToken ) ) {
    _errorLogger.logError( ERR_UNEXPECTED_TOKEN, _token );
  }
}

/* forward declarations */
Node* parseArithmetic( std::stack<Token>& _tokens, ErrorLogger& _errorLogger, const bool _topLevel = false );
Node* parseBinding( std::queue<Token>& _tokens, const Token _variable, ErrorLogger& _errorLogger );
Node* parseStatement( std::queue<Token>& _tokens, ErrorLogger& _errorLogger );

Node* parseConstant( std::stack<Token>& _tokens, ErrorLogger& _errorLogger ) {
  Token constant = _tokens.top();
  _tokens.pop();

  return new ConstantNode( constant );
}

Node* parseVariable( std::stack<Token>& _tokens, ErrorLogger& _errorLogger ) {
  Token variable = _tokens.top();
  _tokens.pop();

  return new VariableNode( variable );
}

Node* parseUnaryOperator( std::stack<Token> & _tokens, ErrorLogger& _errorLogger ) {
  Token uOperator = _tokens.top();
  _tokens.pop();

  ( void ) emptyTokens( _tokens, uOperator, _errorLogger );
  Node* operand = parseArithmetic( _tokens, _errorLogger );

  return new UnaryOperatorNode( uOperator, operand );
}

Node* parseBinaryOperator( std::stack<Token>& _tokens, ErrorLogger& _errorLogger ) {
  Token bOperator = _tokens.top();
  _tokens.pop();

  ( void ) emptyTokens( _tokens, bOperator, _errorLogger );
  Node* rOperand = parseArithmetic( _tokens, _errorLogger );
  ( void ) emptyTokens( _tokens, bOperator, _errorLogger );
  Node* lOperand = parseArithmetic( _tokens, _errorLogger );

  return new BinaryOperatorNode( bOperator, lOperand, rOperand );
}

Node* parseArithmetic( std::stack<Token>& _tokens, ErrorLogger& _errorLogger, const bool _topLevel ) {
  if ( _tokens.empty() ) {
    return nullptr;
  }

  Token token = _tokens.top();
  Node* node = nullptr;

  switch ( token.getType() ) {
    case TokenType::ConstantTokenType:
      node = parseConstant( _tokens, _errorLogger );
      break;
    case TokenType::VariableTokenType:
      node = parseVariable( _tokens, _errorLogger );
      break;
    case TokenType::OperatorTokenType:
      if ( contains( UNARY_OPERATORS, token.getText() ) ) {
        node = parseUnaryOperator( _tokens, _errorLogger );
      } else {
        node = parseBinaryOperator( _tokens, _errorLogger );
      }
      break;
    default:
      _errorLogger.logError( ERR_UNEXPECTED_TOKEN, token );
      break;
  }

  if ( _topLevel && !_tokens.empty() ) {
    _errorLogger.logError( ERR_EXPECTED_TERMINATION, token );
  }

  return node;
}

Node* parseArithmetic( std::queue<Token>& _tokens, ErrorLogger& _errorLogger ) {
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

  return parseArithmetic( organizedTokens, _errorLogger, true );
}

Node* parseBinding( std::queue<Token>& _tokens, ErrorLogger& _errorLogger ) {
  Token bindKeyword = _tokens.front();
  _tokens.pop();
  ( void ) unexpectedToken( bindKeyword, "let", _errorLogger );

  ( void ) emptyTokens( _tokens, bindKeyword, _errorLogger );
  Token variable = _tokens.front();
  _tokens.pop();

  ( void ) emptyTokens( _tokens, bindKeyword, _errorLogger );
  Token typeBinder = _tokens.front();
  _tokens.pop();
  ( void ) unexpectedToken( typeBinder, "::", _errorLogger );

  ( void ) emptyTokens( _tokens, bindKeyword, _errorLogger );
  Token typeBinded = _tokens.front();
  _tokens.pop();

  ( void ) emptyTokens( _tokens, bindKeyword, _errorLogger );
  Token exprBinder = _tokens.front();
  _tokens.pop();
  ( void ) unexpectedToken( exprBinder, "=", _errorLogger );

  ( void ) emptyTokens( _tokens, bindKeyword, _errorLogger );
  Node* bindingExpression = parseStatement( _tokens, _errorLogger );

  return new BindingNode( variable, DATA_TYPE_MAPS[typeBinded.getText()], bindingExpression );
}

Node* parseStatement( std::queue<Token>& _tokens, ErrorLogger& _errorLogger ) {
  if ( _tokens.empty() ) {
    return nullptr;
  }

  Token headToken = _tokens.front();

  switch ( headToken.getType() ) {
    case TokenType::NewlineTokenType:
      _tokens.pop();
      return parseStatement( _tokens, _errorLogger );
      break;
    case TokenType::ConstantTokenType:
    case TokenType::VariableTokenType:
    case TokenType::GroupTokenType:
      return parseArithmetic( _tokens, _errorLogger );
      break;
    case TokenType::KeywordTokenType:
      if ( equals( headToken, "let" ) ) {
        return parseBinding( _tokens, _errorLogger );
      }
      break;
    default:
      ( void ) unexpectedToken( headToken, _errorLogger );
      _tokens.pop();
      break;
  }

  return nullptr;
}

std::queue<Node*> parse( std::queue<Token>& _tokens ) {
  std::queue<Node*> statements;

  ErrorLogger errorLogger;

  Node* statement = parseStatement( _tokens, errorLogger );

  while ( statement && errorLogger.empty() ) {
    statements.push( statement );
    statement = parseStatement( _tokens, errorLogger );
  }

  if ( !errorLogger.empty() ) {
    std::cout << "Kubic encountered the following issues --" << std::endl;
    std::cout << errorLogger.streamErrors().str() << std::endl;
  }

  return statements;
}

#endif