#include <cstdint>
#include <inttypes.h>
#include <iostream>
#include <sstream>

extern "C" uint64_t kubic_main( void );

extern "C" void print( const uint64_t );

std::string unformatValue( const uint64_t _value ) {
  if ( _value == 0x7FFFFFFFFFFFFFFF ) {
    return "false";
  } else if ( _value == 0xFFFFFFFFFFFFFFFF ) {
    return "true";
  } else if ( ( _value & 0x1 ) == 0x0 ) {
    std::ostringstream value;
    value << ( _value >> 1 );
    return value.str();
  } else {
    return "";
  }
}

void error( const uint64_t _errorCode ) {
  exit( 1 );
}

void print( const uint64_t _value ) {
  std::cout << unformatValue( _value ) << std::endl;
}

int main( int argc, char* argv[] ) {
  uint64_t kubicResult = kubic_main();

  return 0;
}