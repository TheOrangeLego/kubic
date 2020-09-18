#ifndef _LEXER_HPP
#define _LEXER_HPP

#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <vector>

#include "parser/token.hpp"
#include "shared/errors.hpp"
#include "shared/utils.hpp"

const std::set<char> WHITESPACES = {
  ' ', '\t',
};

const std::set<char> NUMERIC_BASES = {
  'b', 'o', 'd', 'x'
};

const std::vector<char> NUMERIC_DIGITS = {
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

const std::set<char> OPERATORS = {
  ':', '=',

  /* boolean operators */
  '!',

  /* arithmetic operators */
  '+', '-', '*', '/',
};

const std::set<char> ARITHMETIC_GROUPERS = {
  '(', ')',
};

const std::set<char> STATEMENT_GROUPERS = {
  '{', '}',
};

const std::set<std::string> KEYWORDS = {
  /* value type keywords */
  "integer",

  /* operator keywords */
  "define",

  /* conditional */
  "if", "elif", "else",
};

bool isAlphaLower( const char _char ) {
  return _char >= 'a' && _char <= 'z';
}

bool isAlphaUpper( const char _char ) {
  return _char >= 'A' && _char <= 'Z';
}

bool isAlpha( const char _char ) {
  return isAlphaLower( _char ) || isAlphaUpper( _char );
}

bool isUnderscore( const char _char ) {
  return _char == '_';
}

bool isComma( const char _char ) {
  return _char == ',';
}

bool isNumeric( const char _char, const int _offset = 10 ) {
  std::set<char> validDigits( NUMERIC_DIGITS.begin(), NUMERIC_DIGITS.begin() + _offset );
  return contains( validDigits, _char );
}

bool isValidVariable( const std::string _string ) {
  char head = _string[0];

  if ( _string.length() == 1 ) {
    if ( isAlpha( head ) ) {
      return true;
    } else {
      return false;
    }
  }

  if ( !isAlpha( head ) && !isUnderscore( head ) ) {
    return false;
  }

  for ( char c : _string ) {
    if ( !isAlpha( c ) && !isUnderscore( c ) && !isNumeric( c ) ) {
      return false;
    }
  }

  return true;
}

bool isValidKeyword( const std::string _string ) {
  return contains( KEYWORDS, _string );
}

bool isValidNumeric( const std::string _string, const int _offset = 10, const bool _start = true ) {
  char head = _string[0];

  if ( _string.length() > 2 && _start &&  head == '0' ) {
    char second = _string[1];

    if ( !isNumeric( head, _offset ) && !contains( NUMERIC_BASES, second ) ) {
      return false;
    }

    int offset = 10;

    if ( second == 'b' ) {
      offset = 2;
    } else if ( second == 'o' ) {
      offset = 8;
    } else if ( second == 'd' ) {
      offset = 10;
    } else if ( second == 'x' ) {
      offset = 16;
    }

    return isValidNumeric( _string.substr( 2 ), offset, false );
  }

  for ( char c : _string ) {
    if ( !isNumeric( c, _offset ) ) {
      return false;
    }
  }

  return true;
}

bool isValidOperator( const std::string _string ) {
  for ( char c : _string ) {
    if ( !contains( OPERATORS, c ) ) {
      return false;
    }
  }

  return true;
}

TokenType determineType( const std::string _string ) {
  if ( _string == "false" || _string == "true" ) {
    return TokenType::TokenBoolean;
  } else if ( isValidKeyword( _string ) ) {
    return TokenType::TokenKeyword;
  } else if ( isValidNumeric( _string ) ) {
    return TokenType::TokenConstant;
  } else if ( isValidOperator( _string ) ) {
    return TokenType::TokenOperator;
  } else if ( isValidVariable( _string ) ) {
    return TokenType::TokenVariable;
  } else {
    return TokenType::TokenUndefined;
  }
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void nextChar( unsigned int& _column, unsigned int& _line ) {
  _column++;
}

void nextLine( unsigned int& _column, unsigned int& _line ) {
  _column = 1;
  _line++;
}

std::queue<Token*> tokenize( const std::string _filename ) {
  /* starting line */
  unsigned int line = 1;
  /* starting column */
  unsigned int column = 1;
  /* file buffer */
  std::ifstream file( _filename, std::ios::binary );
  /* read file's contents */
  std::string fileContent(
    ( std::istreambuf_iterator<char>( file ) ), ( std::istreambuf_iterator<char>() )
  );
  
  std::queue<Token*> tokens;

  for ( unsigned int currentIndex = 0; currentIndex < fileContent.length(); currentIndex++ ) {
    char currentChar = fileContent[currentIndex];
    Position position( line, column, _filename );

    if ( contains( WHITESPACES, currentChar ) ) {
      nextChar( column, line );
    } else if ( currentChar == ',' ) {
      tokens.push( new Token( std::string( 1, currentChar ), TokenType::TokenComma, position ) );
      nextChar( column, line );
    } else if ( currentChar == '\n' ) {
      tokens.push( new Token( std::string( 1, currentChar ), TokenType::TokenNewline, position ) );
      nextLine( column, line );
    } else if ( currentChar == '(' || currentChar == ')' ) {
      tokens.push( new Token( std::string( 1, currentChar ), TokenType::TokenArithmeticGrouper, position ) );
    } else if ( currentChar == '{' || currentChar == '}' ) {
      tokens.push( new Token( std::string( 1, currentChar ), TokenType::TokenStatementGrouper, position ) );
    } else {
      unsigned int tokenStart = currentIndex;

      if ( contains( OPERATORS, currentChar ) ) {
        while ( currentIndex < fileContent.length() && contains( OPERATORS, currentChar ) ) {
          currentChar = fileContent[++currentIndex];
          nextChar( column, line );
        }
      } else {
        while (
          currentIndex < fileContent.length()
          && !contains( OPERATORS, currentChar )
          && !contains( WHITESPACES, currentChar )
          && !contains( ARITHMETIC_GROUPERS, currentChar )
          && !contains( STATEMENT_GROUPERS, currentChar )
          && currentChar != '\n'
        ) {
          currentChar = fileContent[++currentIndex];
          nextChar( column, line );
        }
      }

      std::string tokenText = fileContent.substr( tokenStart, currentIndex-- - tokenStart );
      TokenType determinedType = determineType( tokenText );

      if ( determinedType == TokenType::TokenUndefined ) {
        log( Severity::Error, position, ERR_INVALID_TOKEN, tokenText );
      }

      tokens.push( new Token( tokenText, determineType( tokenText ), position ) );
    }
  }

  return tokens;
}

#endif
