#ifndef _TOKENIZER_HPP
#define _TOKENIZER_HPP

#include <string>
#include <vector>
#include "../token/Token.hpp"

static const char WHITESPACES[] = {' ', '\t'};
static const char NEWLINES[] = {'\n', '\r'};
static const char OPERATORS[] = {'+', '-', '*', '/', '='};

static const std::string KEYWORDS[] = {
  "let",
  "in"
};

static bool isWhitespace( const char _char ) {
  bool matches = false;

  for ( char whitespace : WHITESPACES ) {
    matches = matches || whitespace == _char;
  }

  return matches;
}

static bool isNewline( const char _char ) {
  bool matches = false;

  for ( char newline : NEWLINES ) {
    matches = matches || newline == _char;
  }

  return matches;
}

static bool isAlpha( const char _char ) {
  if ( _char >= 'a' && _char <= 'z' ) {
    return true;
  }

  if ( _char >= 'A' && _char <= 'Z' ) {
    return true;
  }

  return false;
}

static bool isNumeric( const char _char ) {
  return _char >= '0' && _char <= '9';
}

static bool isUnderscore( const char _char ) {
  return _char == '_';
}

static bool isOperator( const char _char ) {
  bool matches = false;

  for ( char operatorChar : OPERATORS ) {
    matches = matches || operatorChar == _char;
  }

  return matches;
}

static TokenType deriveType( const std::string _token ) {
  bool isVariable = false;

  for ( unsigned int index = 0; index < _token.length(); index++ ) {
    isVariable = isVariable || isAlpha( _token[index] ) || isUnderscore( _token[index] );
  }

  if ( isVariable ) {
    return TokenType::Variable;
  } else {
    return TokenType::Constant;
  }
  
  return TokenType::Undefined;
}

std::vector<Token> tokenize( const std::string _input ) {
  unsigned int line = 0;
  unsigned int col = 0;
  unsigned int position = 0;
  unsigned int tokenLength = 0;
  unsigned int inputLength = _input.length();
  std::vector<Token> tokens;

  while ( position < inputLength ) {
    while ( isWhitespace( _input[position] ) ) {
      position++;
      col++;
    }

    if ( isNewline( _input[position] ) ) {
      line++;
      position++;
      col = 0;
      continue;
    }

    if ( isAlpha( _input[position] ) || isNumeric( _input[position] ) || isUnderscore( _input[position] ) ) {
      while ( isAlpha( _input[position] ) || isNumeric( _input[position] ) || isUnderscore( _input[position] ) ) {
        position++;
        col++;
        tokenLength++;
      }

      std::string tokenString = _input.substr( position - tokenLength, tokenLength );
      TokenType tokenType = deriveType( tokenString );
      Token newToken( tokenString, tokenType, line + 1, col - tokenLength + 1, DEFAULT_FILENAME );
      tokens.push_back( newToken );
      tokenLength = 0;

      continue;
    }

    if ( isOperator( _input[position] ) ) {
      while ( isOperator( _input[position] ) ) {
        position++;
        col++;
        tokenLength++;
      }

      std::string tokenString = _input.substr( position - tokenLength, tokenLength );
      Token newToken( tokenString, TokenType::Undefined, line + 1, col - tokenLength + 1, DEFAULT_FILENAME );
      tokens.push_back( newToken );
      tokenLength = 0;

      continue;
    }
  }

  if ( tokenLength ) {
    std::string tokenString = _input.substr( position - tokenLength, tokenLength );
    TokenType tokenType = deriveType( tokenString );
    Token lastToken( tokenString, tokenType, line + 1, col - tokenLength + 1, DEFAULT_FILENAME );
    tokens.push_back( lastToken );
  }

  return tokens;
}

#endif
