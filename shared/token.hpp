#ifndef _TOKEN_HPP
#define _TOKEN_HPP

#include <boost/format.hpp>
#include <string>

#include "./types.hpp"

typedef struct {
  unsigned int line;
  unsigned int col;
  std::string filename;
} TokenPosition;

class Token {
  private:
    std::string text;
    TokenType type;
    TokenPosition position;

  public:
    Token(
      const std::string _text,
      const TokenType _type,
      const unsigned int _line,
      const unsigned int _col,
      const std::string _filename
    ) {
      setText( _text );
      setType( _type );
      setPosition( _line, _col, _filename );
    }

    void setText( const std::string _text ) {
      text = _text;
    }

    void setType( const TokenType _type ) {
      type = _type;
    }

    void setPosition( const unsigned int _line, const unsigned int _col, const std::string _filename ) {
      position = { _line, _col, _filename };
    }

    std::string getText() const {
      return text;
    }

    TokenType getType() const {
      return type;
    }

    std::string getPosition() const {
      return ( boost::format( "%1% <%2%,%3%>" ) % getFilename() % getPositionLine() % getPositionCol() ).str();
    }

    unsigned int getPositionLine() const {
      return position.line;
    }

    unsigned int getPositionCol() const {
      return position.col;
    }

    std::string getFilename() const {
      return position.filename;
    }
};

#endif
