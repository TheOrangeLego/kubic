#include "Token.hpp"

Token::Token( const std::string _token ) {
  this->token = _token;
  setType( TokenType::Undefined );
  setPosition(
    DEFAULT_POSITION_LINE,
    DEFAULT_POSITION_COLUMN,
    DEFAULT_FILENAME
  );
}

Token::Token(
  const std::string _token,
  const TokenType _type,
  const unsigned int _line, const unsigned int _col, const std::string _filename
) {
  this->token = _token;
  setPosition( _line, _col, _filename );
  setType( _type );
}

void Token::setType( const TokenType _type ) {
  this->type = _type;
}

void Token::setPosition( const unsigned int _line, const unsigned int _col, const std::string _filename ) {
  this->position = { _line, _col, _filename };
}

std::string Token::getToken() const {
  return this->token;
}

TokenType Token::getType() const {
  return this->type;
}

TokenPosition Token::getPosition() const {
  return this->position;
}

unsigned int Token::getPositionLine() const {
  return this->position.line;
}

unsigned int Token::getPositionCol() const {
  return this->position.col;
}

std::string Token::getFilename() const {
  return this->position.filename;
}
