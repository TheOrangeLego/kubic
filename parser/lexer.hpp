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
  unsigned int _line,
  unsigned int& _col,
  unsigned int& _position,
  std::string _filename,
  unsigned int& _tokenLength,
  unsigned int _inputLength,
  TokenType _type
) {
  char currentChar = _input[_position];

  if ( _type == TokenType::TokenGroup ) {
    std::string tokenString( 1, currentChar );
    _col++;
    _position++;

    return Token( tokenString, _type, _line, _col, _filename );
  }

  while ( _position < _inputLength && tokenizeConditions[_type]( currentChar ) ) {
    _tokenLength++;
    _col++;
    currentChar = _input[++_position];
  }

  if ( _type ==  TokenType::TokenKeyword && ( isConstant( currentChar ) || isUnderscore( currentChar ) ) ) {
    return tokenizeItem(
      _input, _line, _col, _position, _filename, _tokenLength, _inputLength, TokenType::TokenVariable
    );
  }
  
  if ( _type != TokenType::TokenOperator && _position < _inputLength &&
       !isWhitespace( currentChar ) && !isNewline( currentChar ) && !isOperator( currentChar ) ) {
    return tokenizeItem(
      _input, _line, _col, _position, _filename, _tokenLength, _inputLength, TokenType::TokenUndefined
    );
  }

  std::string tokenString = _input.substr( _position - _tokenLength, _tokenLength );

  if ( _type == TokenType::TokenKeyword && contains( CONSTANT_KEYWRDS, tokenString ) ) {
    _type = TokenType::TokenConstant;
  }

  if ( _type == TokenType::TokenKeyword && !contains( KEYWORDS, tokenString ) ) {
    _type = TokenType::TokenVariable;
  }

  Token token( tokenString, _type, _line, _col, _filename );
  _tokenLength = 0;
  return token;
}

std::queue<Token> tokenize( const std::string _input, const std::string _filename ) {
  unsigned int line = 0;
  unsigned int col = 0;
  unsigned int position = 0;
  unsigned int tokenLength = 0;
  unsigned int inputLength = _input.length();
  std::queue<Token> tokens;

  while ( position < inputLength ) {
    char currentChar = _input[position];

    if ( isWhitespace( currentChar ) ) {
      position++;
      col++;
    } else if ( isNewline( currentChar ) ) {
      tokens.push( Token( std::string( 1, currentChar ), TokenType::TokenNewline, line++, col, _filename ) );
      position++;
      col = 0;
    } else {
      Token token = isGroup( currentChar ) 
        ? tokenizeItem( _input, line, col, position, _filename, tokenLength, inputLength, TokenType::TokenGroup )
        : isOperator( currentChar )
          ? tokenizeItem( _input, line, col, position, _filename, tokenLength, inputLength, TokenType::TokenOperator )
          : isConstant( currentChar )
            ? tokenizeItem( _input, line, col, position, _filename, tokenLength, inputLength, TokenType::TokenConstant )
            : isUnderscore( currentChar )
              ? tokenizeItem( _input, line, col, position, _filename, tokenLength, inputLength, TokenType::TokenVariable )
              : tokenizeItem( _input, line, col, position, _filename, tokenLength, inputLength, TokenType::TokenKeyword );
      tokens.push( token );
    }
  }

  return tokens;
}

#endif
