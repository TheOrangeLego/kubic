#ifndef _LEXER_HPP
#define _LEXER_HPP

#include <map>
#include <queue>
#include <string>

#include "shared/helpers.hpp"
#include "parser/rules.hpp"
#include "shared/token.hpp"

static bool isAlphaLower( const char _char ) {
  return _char >= 'a' && _char <= 'z';
}

static bool isAlphaUpper( const char _char ) {
  return _char >= 'A' && _char <= 'Z';
}

static bool isAlpha( const char _char ) {
  return isAlphaLower( _char ) || isAlphaUpper( _char );
}

static bool isUnderscore( const char _char ) {
  return _char == '_';
}

static bool isConstant( const char _char ) {
  return _char >= '0' && _char <= '9';
}

static bool isWhitespace( const char _char ) {
  return contains( WHITESPACE_CHARS, _char );
}

static bool isNewline( const char _char ) {
  return contains( NEWLINE_CHARS, _char );
}

static bool isOperator( const char _char ) {
  return contains( OPERATOR_CHARS, _char );
}

static bool isGroup( const char _char ) {
  return contains( GROUP_CHARS, _char );
}

static bool tokenizeConstant( const char _char ) {
  return isConstant( _char ) ||
    _char == 'x' || _char == 'o' || ( _char >= 'a' && _char <= 'f' ) || ( _char >= 'A' && _char <= 'F' );
}

static bool tokenizeVariable( const char _char ) {
  return isAlpha( _char ) || isConstant( _char ) || isUnderscore( _char );
}

static bool tokenizeKeyword( const char _char ) {
  return isAlpha( _char );
}

static bool tokenizeOperator( const char _char ) {
  return isOperator( _char );
}

static bool tokenizeGroup( const char _char ) {
  return isGroup( _char );
}

static bool tokenizeUndefined( const char _char ) {
  return !isNewline( _char ) && !isWhitespace( _char ) && !isOperator( _char );
}

static std::map<TokenType, bool (*)(char)> tokenizeConditions = {
  { TokenType::TokenUndefined, tokenizeUndefined },
  { TokenType::TokenConstant,  tokenizeConstant },
  { TokenType::TokenVariable,  tokenizeVariable },
  { TokenType::TokenGroup,     tokenizeGroup },
  { TokenType::TokenKeyword,   tokenizeKeyword },
  { TokenType::TokenOperator,  tokenizeOperator }
};

/*
 * single item parsing logic - each type of token has a criteria of what characters can appear at what
 *   locations. If any of these criterias are violated, then we are either parsing another type of
 *   token or it is an undefined/invalid token. Below are all the criterias for each token type:
 * 
 * 1 - Keyword  - only alphabetical characters allowed
 * 2 - Variable - alphanumerical and underscore allowed, where it cannot start with a digit
 * 3 - Operator - only valid operator characters allowed
 * 4 - Constant - digits, letters a-f/A-F for hexadecimal, and 'o', 'd', and 'x' to explicitly denote
 *                whether the digit is octal, decimal, or hexal, respectively
 */
static Token tokenizeItem(
  std::string _input,
  Position _position,
  unsigned int& _absolutePosition,
  unsigned int& _tokenLength,
  unsigned int _inputLength,
  TokenType _type
) {
  char currentChar = _input[_absolutePosition];

  if ( _type == TokenType::TokenGroup ) {
    std::string tokenString( 1, currentChar );
    _position.space();
    _absolutePosition++;

    return Token( tokenString, _type, _position );
  }

  while ( _absolutePosition < _inputLength && tokenizeConditions[_type]( currentChar ) ) {
    _tokenLength++;
    _position.space();
    currentChar = _input[++_absolutePosition];
  }

  if ( _type ==  TokenType::TokenKeyword && ( isConstant( currentChar ) || isUnderscore( currentChar ) ) ) {
    return tokenizeItem(
      _input, _position, _absolutePosition, _tokenLength, _inputLength, TokenType::TokenVariable
    );
  }
  
  if (
      _type != TokenType::TokenOperator
      && _absolutePosition < _inputLength
      && !isWhitespace( currentChar )
      && !isNewline( currentChar )
      && !isOperator( currentChar )
      && !isGroup( currentChar )
  ) {
    return tokenizeItem(
      _input, _position, _absolutePosition, _tokenLength, _inputLength, TokenType::TokenUndefined
    );
  }

  std::string tokenString = _input.substr( _absolutePosition - _tokenLength, _tokenLength );
  bool (*digitChecker)(char) = []( char _digit ){ return _digit >= '0' && _digit <= '9'; };
  unsigned int startIndex = 0;

  if ( _type == TokenType::TokenConstant ) {
    if ( tokenString.substr( 0, 2 ) == "0b" ) {
      digitChecker = []( char _digit ){ return _digit >= '0' && _digit <= '1'; };
      startIndex = 2;
    } else if ( tokenString.substr( 0, 2 ) == "0o" ) {
      digitChecker = []( char _digit ){ return _digit >= '0' && _digit <= '7'; };
      startIndex = 2;
    } else if ( tokenString.substr( 0, 2 ) == "0x" ) {
      digitChecker = []( char _digit ){
        return ( _digit >= '0' && _digit <= '9' )
          || ( _digit >= 'a' && _digit <= 'f' )
          || ( _digit >='A' && _digit <= 'F' );
        };
      startIndex = 2;
    }

    for ( ; startIndex < tokenString.length(); startIndex++ ) {
      if ( !digitChecker( tokenString[startIndex] ) ) {
        _type = TokenType::TokenUndefined;
        break;
      }
    }
  }

  if ( _type == TokenType::TokenKeyword && contains( CONSTANT_KEYWRDS, tokenString ) ) {
    _type = TokenType::TokenConstant;
  }

  if ( _type == TokenType::TokenKeyword && !contains( KEYWORDS, tokenString ) ) {
    _type = TokenType::TokenVariable;
  }

  Token token( tokenString, _type, _position );
  _tokenLength = 0;
  return token;
}

std::queue<Token> tokenize( const std::string _input, const std::string _filename ) {
  Position position( 0, 0, _filename );
  unsigned int absolutePosition = 0;
  unsigned int tokenLength = 0;
  unsigned int inputLength = _input.length();
  std::queue<Token> tokens;

  while ( absolutePosition < inputLength ) {
    char currentChar = _input[absolutePosition];

    if ( isWhitespace( currentChar ) ) {
      absolutePosition++;
      position.space();
    } else if ( isNewline( currentChar ) ) {
      tokens.push( Token( std::string( 1, currentChar ), TokenType::TokenNewline, position ) );
      position.newline();
      absolutePosition++;
    } else {
      Token token = isGroup( currentChar ) 
        ? tokenizeItem( _input, position, absolutePosition, tokenLength, inputLength, TokenType::TokenGroup )
        : isOperator( currentChar )
          ? tokenizeItem( _input, position, absolutePosition, tokenLength, inputLength, TokenType::TokenOperator )
          : isConstant( currentChar )
            ? tokenizeItem( _input, position, absolutePosition, tokenLength, inputLength, TokenType::TokenConstant )
            : isUnderscore( currentChar )
              ? tokenizeItem( _input, position, absolutePosition, tokenLength, inputLength, TokenType::TokenVariable )
              : tokenizeItem( _input, position, absolutePosition, tokenLength, inputLength, TokenType::TokenKeyword );
      tokens.push( token );
    }
  }

  return tokens;
}

#endif
