#ifndef _TOKENIZER_HPP
#define _TOKENIZER_HPP

#include <string>
#include <queue>

#include "rules.hpp"
#include "helpers.hpp"
#include "token/Token.hpp"

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

static bool parsingConstant( const char _char ) {
  return isNumeric( _char ) || _char == 'x' || _char == 'o' ||
    ( _char >= 'a' && _char <= 'z' ) ||
    ( _char >= 'A' && _char <= 'Z' );
}

static bool parsingVariable( const char _char ) {
  return isAlpha( _char ) || isNumeric( _char ) || isUnderScore( _char );
}

static bool parsingKeyword( const char _char ) {
  return isAlpha( _char );
}

static bool parsingOperator( const char _char ) {
  return isOperator( _char );
}

static bool parsingUndefined( const char _char ) {
  return !isNewline( _char ) && !isWhitespace( _char ) && !isOperator( _char );
}

static std::map<TokenType, bool (*)(char)> parsingConditions = {
  {TokenType::Constant,  parsingConstant},
  {TokenType::Variable,  parsingVariable},
  {TokenType::Keyword,   parsingKeyword},
  {TokenType::Operator,  parsingOperator},
  {TokenType::Undefined, parsingUndefined}
};

static Token tokenizeItem( const std::string _input, const unsigned int _line, unsigned int& _col,
  unsigned int& _position, unsigned int& _tokenLength, const unsigned int _inputLength,
  const TokenType _type ) {
  char currentChar = _input[_position];

  if ( currentChar == '(' || currentChar == ')' ) {
    std::string tokenString( 1, currentChar );
    TokenType type = currentChar == '(' ? TokenType::LeftGroup : TokenType::RightGroup;
    _col++;
    _position++;

    return Token( tokenString, type, _line, _col, DEFAULT_FILENAME );
  }

  while ( _position < _inputLength && parsingConditions[_type]( currentChar ) ) {
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
  unsigned int col  = 0;
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
      Token token = isOperator( currentChar )
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
