#ifndef _TOKENIZER_HPP
#define _TOKENIZER_HPP

#include <string>
#include <vector>
#include "../token/Token.hpp"

const char WHITESPACE[] = {' ', '\t'};
const char NEWLINE[] = {'\n', '\r'};
const char OPERATORS[] = {'+', '-', '*', '/', '='};
bool isWhitespace( const char _char ) {
  for ( char whitespace : WHITESPACE ) {
    if ( whitespace == _char ) {
      return true;
    }
  }

  return false;
}

bool isNewline( const char _char ) {
  for ( char newline : NEWLINE ) {
    if ( newline == _char ) {
      return true;
    }
  }

  return false;
}

bool isAlpha( const char _char ) {
  if ( _char >= 'a' && _char <= 'z' ) {
    return true;
  }

  if ( _char >= 'A' && _char <= 'Z' ) {
    return true;
  }

  return false;
}

bool isNumeric( const char _char ) {
  return _char >= '0' && _char <= '9';
}

bool isUnderscore( const char _char ) {
  return _char == '_';
}

bool isOperator( const char _char ) {
  for ( char operatorChar : OPERATORS ) {
    if ( operatorChar == _char ) {
      return true;
    }
  }

  return false;
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
      Token newToken( tokenString, line + 1, col - tokenLength + 1, DEFAULT_FILENAME );
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
      Token newToken( tokenString, line + 1, col - tokenLength + 1, DEFAULT_FILENAME );
      tokens.push_back( newToken );
      tokenLength = 0;

      continue;
    }
  }

  if ( tokenLength ) {
    std::string tokenString = _input.substr( position - tokenLength, tokenLength );
    Token lastToken( tokenString, line + 1, col - tokenLength + 1, DEFAULT_FILENAME );
    tokens.push_back( lastToken );
  }

  return tokens;
}

#endif
