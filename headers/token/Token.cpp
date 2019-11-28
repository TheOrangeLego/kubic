#include "Token.hpp"

Token::Token( const std::string _token ) {
  this->token = _token;
  setPosition(
    DEFAULT_POSITION_LINE,
    DEFAULT_POSITION_COLUMN,
    DEFAULT_FILENAME
  );
}

Token::Token(
  const std::string _token,
  const unsigned int _line,
  const unsigned int _col,
  const std::string _filename
) {
  this->token = _token;
  setPosition( _line, _col, _filename );
}

void Token::setPosition( const unsigned int _line, const unsigned int _col, const std::string _filename ) {
  this->position = { _line, _col, _filename };
}

std::string Token::getToken() const {
  return this->token;
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
