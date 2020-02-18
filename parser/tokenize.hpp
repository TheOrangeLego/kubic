#ifndef _TOKENIZE_HPP
#define _TOKENIZE_HPP

#include <queue>

#include "rules.hpp"
#include "../shared/helpers.hpp"
#include "../shared/token/Token.hpp"

static bool isAlphaLower( const char _char ) {
  return _char >= 'a' && _char <= 'z';
}

static bool isAlphaUpper( const char _char ) {
  return _char >= 'A' && _char <= 'Z';
}

static bool isAlpha( const char _char ) {
  return isAlphaLower( _char ) || isAlphaUpper( _char );
}

static bool isUnderScore( const char _char ) {
  return _char == '_';
}

static bool isNumeric( const char _char ) {
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

static bool isGrouper( const char _char ) {
  return contains( GROUP_CHARS, _char );
}

static bool tokenizeConstant( const char _char ) {
  return isNumeric( _char ) || _char == 'x' || _char == 'o' ||
    ( _char >= 'a' && _char <= 'f' ) || ( _char >= 'A' && _char <= 'F' );
}

static bool tokenizeVariable( const char _char ) {
  return isAlpha( _char ) || isNumeric( _char ) || isUnderScore( _char );
}

static bool tokenizeKeyword( const char _char ) {
  return isAlpha( _char );
}

static bool tokenizeOperator( const char _char ) {
  return isOperator( _char );
}

static bool tokenizeGrouper( const char _char ) {
  return isGrouper( _char );
}

static bool tokenizeUndefined( const char _char ) {
  return !isNewline( _char ) && !isWhitespace( _char ) && !isOperator( _char );
}

static std::map<TokenType, bool (*)(char)> tokenizeConditions = {
  {TokenType::Constant,  tokenizeConstant},
  {TokenType::Variable,  tokenizeVariable},
  {TokenType::Keyword,   tokenizeKeyword},
  {TokenType::Operator,  tokenizeOperator},
  {TokenType::Grouper,   tokenizeGrouper},
  {TokenType::Undefined, tokenizeUndefined}
};

/*
 * single item parsing logic - each type of token has a criteria of what characters can appear at what
 *   locations. If any of these criterias are violated, then we are either parsing another type of
 *   token or is an undefined ( invalid ) token. Below are all the criterias for each token type:
 * 
 * 1 - Keyword - only alphabetical characters allowed
 * 2 - Variable - alphanumerical and underscore allowed, where it cannot start with a digit
 * 3 - Operator - only valid operator characters allowed
 * 4 - Constant - digits, letters a-f/A-F for hexadecimal, and 'o', 'd', and 'x' to explicitly denote
 *                whether the digit is octal, decimal, or hexal, respectively
 */
static Token tokenizeItem( const std::string _input, const unsigned int _line, unsigned int& _col,
  unsigned int& _position, unsigned int& _tokenLength, const unsigned int _inputLength,
  const TokenType _type ) {
  char currentChar = _input[_position];

  if ( _type == TokenType::Grouper ) {
    std::string tokenString( 1, currentChar );
    _col++;
    _position++;

    return Token( tokenString, _type, _line, _col, DEFAULT_FILENAME );
  }

  while ( _position < _inputLength && tokenizeConditions[_type]( currentChar ) ) {
    _tokenLength++;
    _col++;
    currentChar = _input[++_position];
  }

  if ( _type ==  TokenType::Keyword && ( isNumeric( currentChar ) || isUnderScore( currentChar ) ) ) {
    return tokenizeItem( _input, _line, _col, _position, _tokenLength, _inputLength, TokenType::Variable );
  }
  
  if ( _type != TokenType::Operator && _position < _inputLength &&
       !isWhitespace( currentChar ) && !isNewline( currentChar ) && !isOperator( currentChar ) ) {
    return tokenizeItem( _input, _line, _col, _position, _tokenLength, _inputLength, TokenType::Undefined );
  }

  std::string tokenString = _input.substr( _position - _tokenLength, _tokenLength );

  if ( _type == TokenType::Keyword && KEYWORDS.find( tokenString ) == KEYWORDS.end() ) {
    return tokenizeItem( _input, _line, _col, _position, _tokenLength, _inputLength, TokenType::Variable );
  }

  Token token( tokenString, _type, _line, _col, DEFAULT_FILENAME );
  _tokenLength = 0;
  return token;
}

std::queue<Token> tokenize( const std::string _input ) {
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
      line++;
      position++;
      col = 0;
    } else {
      Token token = isGrouper( currentChar ) 
        ? tokenizeItem( _input, line, col, position, tokenLength, inputLength, TokenType::Grouper )
        : isOperator( currentChar )
          ? tokenizeItem( _input, line, col, position, tokenLength, inputLength, TokenType::Operator )
          : isNumeric( currentChar )
            ? tokenizeItem( _input, line, col, position, tokenLength, inputLength, TokenType::Constant )
            : isUnderScore( currentChar )
              ? tokenizeItem( _input, line, col, position, tokenLength, inputLength, TokenType::Variable )
              : tokenizeItem( _input, line, col, position, tokenLength, inputLength, TokenType::Keyword );
      tokens.push( token );
    }
  }

  return tokens;
}

#endif
