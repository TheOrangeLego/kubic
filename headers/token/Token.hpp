#ifndef _TOKEN_HPP
#define _TOKEN_HPP

#include <string>

const unsigned int DEFAULT_POSITION_LINE   = 0;
const unsigned int DEFAULT_POSITION_COLUMN = 0;
const std::string  DEFAULT_FILENAME = "#NO_FILE#";

typedef struct {
  unsigned int line;
  unsigned int col;
  std::string filename;
} TokenPosition;

class Token {
  private:
    std::string token;
    TokenPosition position;

  public:
    Token( const std::string _token );
    Token(
      const std::string _token,
      const unsigned int _line,
      const unsigned int _col,
      const std::string _filename
    );

    void setPosition(
      const unsigned int _line,
      const unsigned int _col,
      const std::string _filename
    );

    std::string getToken() const;
    TokenPosition getPosition() const;
    unsigned int getPositionLine() const;
    unsigned int getPositionCol() const;
    std::string getFilename() const;
};

#endif
