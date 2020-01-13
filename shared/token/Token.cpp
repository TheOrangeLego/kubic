#include "Token.hpp"

#include <boost/format.hpp>

Token::Token(
  const std::string _token,
  const TokenType _type,
  const unsigned int _line,
  const unsigned int _col,
  const std::string _filename
) {
  setToken( _token );
  setType( _type );
  setPosition( _line, _col, _filename );
}

void Token::setToken( const std::string _token ) {
  this->token = _token;
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

std::string Token::getPosition() const {
  return ( boost::format( "%1% <%2%,%3%>" ) % this->position.filename % this->position.line %
    this->position.col ).str();
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
