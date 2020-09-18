#ifndef _POSITION_HPP
#define _POSITION_HPP

#include <boost/format.hpp>
#include <string>

class Position {
  private:
    unsigned int line;
    unsigned int column;
    std::string filename;

  public:
    Position( const unsigned int _line, const unsigned _column, const std::string _filename )
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

    std::string getPosition() const {
      return ( boost::format( "[%1% @ %2%.%3%]" ) % filename % line % column ).str();
    }
};

#endif
