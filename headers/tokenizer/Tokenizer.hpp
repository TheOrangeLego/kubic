#ifndef _TOKENIZER_HPP
#define _TOKENIZER_HPP

#include <string>
#include <vector>
#include "../token/Token.hpp"

static const char WHITESPACE_CHARS[] = {
  ' ',
  '\t'
};

static const char NEWLINE_CHARS[] = {
  '\n',
  '\r'
};

static const char OPERATOR_CHARS[] = {
  '+',
  '-',
  '*',
  '/'
};

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
  bool matches = false;

  for ( char whitespaceChar : WHITESPACE_CHARS ) {
    matches = matches || whitespaceChar == _char;
  }

  return matches;
}

static bool isNewline( const char _char ) {
  bool matches = false;

  for ( char newlineChar : NEWLINE_CHARS ) {
    matches = matches || newlineChar == _char;
  }

  return matches;
}

static bool isOperator( const char _char ) {
  bool matches = false;

  for ( char operatorChar : OPERATOR_CHARS ) {
    matches = matches || operatorChar == _char;
  }

  return matches;
}

/*
 * TODO --
 *   these tokenize functions all share the same general functionality -> use function pointers, i.e.
 *     - get the current char
 *     - check that it meets a certain criteria, and if so repeat with step above with next character
 *     - once condition is not met, exit from loop
 *     - if next char meets another finer rule, then pass on to another tokenizer
 *     - o/w if next char is not a space or operator type character, then it must be undefined
 *     - extract string, reset token length, and return extracted token
 */

static Token tokenizeUndefined( const std::string _input, const unsigned int _line, unsigned int& _col,
  unsigned int& _position, unsigned int& _tokenLength, const unsigned int _inputLength ) {
  char currentChar = _input[_position];

  while ( _position < _inputLength &&
    ( !isNewline( currentChar ) && !isWhitespace( currentChar ) && !isOperator( currentChar ) ) ) {
    _tokenLength++;
    _col++;
    currentChar = _input[++_position];
  }

  std::string tokenString = _input.substr( _position - _tokenLength, _tokenLength );
  Token token( tokenString, TokenType::Undefined, _line, _col, DEFAULT_FILENAME );
  _tokenLength = 0;
  return token;
}

static Token tokenizeOperator( const std::string _input, const unsigned int _line, unsigned int& _col,
  unsigned int& _position, unsigned int& _tokenLength, const unsigned int _inputLength ) {
  char currentChar = _input[_position];

  while ( _position < _inputLength && isOperator( currentChar ) ) {
   _tokenLength++;
   _col++;
   currentChar = _input[++_position];
  }

  std::string tokenString = _input.substr( _position - _tokenLength, _tokenLength );
  Token token( tokenString, TokenType::Operator, _line, _col, DEFAULT_FILENAME );
  _tokenLength = 0;
  return token;
}

static Token tokenizeNumber( const std::string _input, const unsigned int _line, unsigned int& _col,
  unsigned int& _position, unsigned int& _tokenLength, const unsigned int _inputLength ) {
    char currentChar = _input[_position];

    while ( _position < _inputLength && ( isNumeric( currentChar ) || currentChar == 'x' || currentChar == 'o'
      || ( currentChar >= 'a' && currentChar <= 'f' )
      || ( currentChar >= 'A' && currentChar <= 'F' ) ) ) {
      _tokenLength++;
      _col++;
      currentChar = _input[++_position];
    }

    if ( !isWhitespace( currentChar ) && !isNewline( currentChar ) && !isOperator( currentChar ) ) {
      return tokenizeUndefined( _input, _line, _col, _position, _tokenLength, _inputLength );
    }

    std::string tokenString = _input.substr( _position - _tokenLength, _tokenLength );
    Token token( tokenString, TokenType::Constant, _line, _col, DEFAULT_FILENAME );
    _tokenLength = 0;
    return token;
}

static Token tokenizeVariable( const std::string _input, const unsigned int _line, unsigned int& _col,
  unsigned int& _position, unsigned int& _tokenLength, const unsigned int _inputLength ) {
  char currentChar = _input[_position];

  while ( _position < _inputLength &&
    ( isAlpha( currentChar ) || isNumeric( currentChar ) || isUnderScore( currentChar ) ) ) {
    _tokenLength++;
    _col++;
    currentChar = _input[++_position];
  }

  if ( !isWhitespace( currentChar ) && !isNewline( currentChar ) && !isOperator( currentChar ) ) {
    return tokenizeUndefined( _input, _line, _col, _position, _tokenLength, _inputLength );
  }

  std::string tokenString = _input.substr( _position - _tokenLength, _tokenLength );
  Token token( tokenString, TokenType::Constant, _line, _col, DEFAULT_FILENAME );
  _tokenLength = 0;
  return token;
}

static Token tokenizeKeyword( const std::string _input, const unsigned int _line, unsigned int& _col,
  unsigned int& _position, unsigned int& _tokenLength, const unsigned int _inputLength ) {
  char currentChar = _input[_position];

  while ( _position < _inputLength && isAlpha( currentChar ) ) {
    _tokenLength++;
    _col++;
    currentChar = _input[++_position];
  }

  if ( isNumeric( currentChar ) || isUnderScore( currentChar ) ) {
    return tokenizeVariable( _input, _line, _col, _position, _tokenLength, _inputLength );
  }

  if ( !isWhitespace( currentChar ) && !isNewline( currentChar ) && !isOperator( currentChar ) ) {
    return tokenizeUndefined( _input, _line, _col, _position, _tokenLength, _inputLength );
  }

  std::string tokenString = _input.substr( _position - _tokenLength, _tokenLength );
  Token token( tokenString, TokenType::Constant, _line, _col, DEFAULT_FILENAME );
  _tokenLength = 0;
  return token;
}

std::vector<Token> tokenize( const std::string _input ) {
  unsigned int line = 0;
  unsigned int col = 0;
  unsigned int position = 0;
  unsigned int tokenLength = 0;
  unsigned int inputLength = _input.length();
  std::vector<Token> tokens;

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
        ? tokenizeOperator( _input, line, col, position, tokenLength, inputLength )
        : isNumeric( currentChar )
          ? tokenizeNumber( _input, line, col, position, tokenLength, inputLength )
          : isUnderScore( currentChar )
            ? tokenizeVariable( _input, line, col, position, tokenLength, inputLength )
            : tokenizeKeyword( _input, line, col, position, tokenLength, inputLength );
      tokens.push_back( token );
    }
  }

  if ( tokenLength ) {
    std::string tokenString = _input.substr( position - tokenLength, tokenLength );
    Token lastToken( tokenString, TokenType::Undefined, line, col - tokenLength, DEFAULT_FILENAME );
    tokens.push_back( lastToken );
  }

  return tokens;
}

#endif