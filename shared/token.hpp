#ifndef _TOKEN_HPP
#define _TOKEN_HPP

#include <boost/format.hpp>
#include <string>

#include "shared/helpers.hpp"
#include "shared/types.hpp"

class Position {
  protected:
    unsigned int line;
    unsigned int column;
    std::string filename;

  public:
    Position( unsigned int _line = 0, unsigned int _column = 0, std::string _filename = "" )
      : line( _line ), column( _column ), filename( _filename ) {}

    unsigned int getLine() const {
      return line;
    }

    unsigned int getColumn() const {
      return column;
    }

    std::string getFilename() const {
      return filename;
    }

    void setPosition( const unsigned int _line, const unsigned int _column, const std::string _filename ) {
      line = _line;
      column = _column;
      filename = _filename;
    }

    void space() {
      column++;
    }

    void newline() {
      line++;
      column = 0;
    }
};

class Token {
  private:
    std::string text;
    TokenType type;
    Position position;

  public:
    Token() {}

    Token(
      const std::string _text,
      const TokenType _type,
      const Position _position
    ) {
      setText( _text );
      setType( _type );
      position = _position;
    }

    void setText( const std::string _text ) {
      text = _text;
    }

    void setType( const TokenType _type ) {
      type = _type;
    }

    std::string getText() const {
      return text;
    }

    TokenType getTokenType() const {
      return type;
    }

    std::string getPosition() const {
      return ( boost::format( "%1% @ <%2%,%3%>" ) % getFilename() % getPositionLine() % getPositionColumn() ).str();
    }

    unsigned int getPositionLine() const {
      return position.getLine();
    }

    unsigned int getPositionColumn() const {
      return position.getColumn();
    }

    std::string getFilename() const {
      return position.getFilename();
    }
};

bool operator< ( const Token& _tokenA, const Token& _tokenB ) {
  return _tokenA.getText() < _tokenB.getText();
}

bool operator== ( const Token& _tokenA, const Token& _tokenB ) {
  return _tokenA.getText() == _tokenB.getText();
}

bool operator== ( const Token& _token, const std::string _string ) {
  return _token.getText() == _string;
}

bool operator!= ( const Token& _token, const std::string _string ) {
  return !( _token == _string );
}

bool operator== ( const Token& _token, const TokenType _type ) {
  return _token.getTokenType() == _type;
}

#endif
