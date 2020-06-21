#ifndef _PARSER_HPP
#define _PARSER_HPP

#include <queue>
#include <stack>
#include <vector>

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
  if ( _token != _expectedToken ) {
    _errorLogger.logError( ERR_EXPECTED_TOKEN, _token, _expectedToken );
  }
}

/* forward declarations */
Node* parseArithmetic( std::stack<Token>& _tokens, ErrorLogger& _errorLogger, const bool _topLevel = false );
Node* parseBinding( std::queue<Token>& _tokens, const Token _variable, ErrorLogger& _errorLogger );
Node* parseStatement( std::queue<Token>& _tokens, ErrorLogger& _errorLogger );

Node* parseConstant( std::stack<Token>& _tokens, ErrorLogger& _errorLogger ) {
  Token constant = _tokens.top();
  _tokens.pop();

  if ( constant == "true" ) {
    return new BooleanNode( constant );
  } else if ( constant == "false" ) {
    return new BooleanNode( constant );
  } else {
    return new IntegerNode( constant );
  }
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

  switch ( token.getTokenType() ) {
    case TokenType::TokenConstant:
      node = parseConstant( _tokens, _errorLogger );
      break;
    case TokenType::TokenVariable:
      node = parseVariable( _tokens, _errorLogger );
      break;
    case TokenType::TokenOperator:
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
  bool continueParsing = true;

  while ( continueParsing && !_tokens.empty() ) {
    headToken = _tokens.front();

    switch ( headToken.getTokenType() ) {
      case TokenType::TokenConstant:
      case TokenType::TokenVariable:
        organizedTokens.push( headToken );
        break;
      case TokenType::TokenGroup:
        if ( headToken == "(" ) {
          operatorTokens.push( headToken );
        } else if ( headToken == ")" ) {
          topOperatorToken = operatorTokens.top();

          while ( !operatorTokens.empty() && topOperatorToken != "(" ) {
            topOperatorToken = operatorTokens.top();
            organizedTokens.push( topOperatorToken );
            operatorTokens.pop();
          }

          operatorTokens.pop();
        } else {
          continueParsing = false;
        }
        break;
      case TokenType::TokenOperator:
        if ( !operatorTokens.empty() ) {
          topOperatorToken = operatorTokens.top();
        }

        while ( !operatorTokens.empty() && !hasHigherPriority( headToken, topOperatorToken ) ) {
          topOperatorToken = operatorTokens.top();
          organizedTokens.push( topOperatorToken );
          operatorTokens.pop();
        }

        operatorTokens.push( headToken );
        break;
      default:
        continueParsing = false;
        break;
    }

    if ( continueParsing ) _tokens.pop();
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

  return new BindingNode( variable, typeBinded, bindingExpression );
}

Node* parseMultiStatements( std::queue<Token>& _tokens, ErrorLogger& _errorLogger ) {
  Token headToken;
  _tokens.pop();

  std::vector<Node*> statements;

  while ( !_tokens.empty() ) {
    headToken = _tokens.front();
    if ( headToken == TokenType::TokenNewline ) {
      _tokens.pop();
    } else if ( headToken != "}" ) {
      statements.push_back( parseStatement( _tokens, _errorLogger ) );
    } else {
      _tokens.pop();
      return new MultiStatementNode( statements );
    }
  }

  _errorLogger.logError( ERR_UNEXPECTED_TERMINATION, headToken );
  return nullptr;
}

Node* parseStatement( std::queue<Token>& _tokens, ErrorLogger& _errorLogger ) {
  if ( _tokens.empty() ) {
    return nullptr;
  }

  Token headToken = _tokens.front();

  switch ( headToken.getTokenType() ) {
    case TokenType::TokenNewline:
      _tokens.pop();
      return parseStatement( _tokens, _errorLogger );
      break;
    case TokenType::TokenConstant:
    case TokenType::TokenVariable:
      return parseArithmetic( _tokens, _errorLogger );
      break;
    case TokenType::TokenGroup:
      if ( headToken == "(" ) {
        return parseArithmetic( _tokens, _errorLogger );
      } else if ( headToken == "{" ) {
        return parseMultiStatements( _tokens, _errorLogger );
      } else {
        _errorLogger.logError( ERR_UNEXPECTED_TOKEN, headToken );
        _tokens.pop();
      }
      break;
    case TokenType::TokenKeyword:
      if ( headToken == "let" ) {
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
    std::cout << "Kubic encountered the following issues --" << std::endl << errorLogger.getErrors();
    std::cout << "Total errors encountered: " << errorLogger.getErrorCount() << std::endl;
  }

  return statements;
}

#endif
