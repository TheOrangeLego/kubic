#include <iostream>

extern "C" unsigned long long int kubic_main( void );

std::string unformatValue( const unsigned long long int _value ) {
  if ( _value == 0x7FFFFFFFFFFFFFFF ) {
    return "false";
  } else if ( _value == 0xFFFFFFFFFFFFFFFF ) {
    return "true";
  } else if ( ( _value & 0x1 ) == 0x0 ) {
    return std::to_string( _value >> 1 );
  } else {
    return "";
  }
}

void error( unsigned long long int _errorCode ) {
  exit( 1 );
}

void print( unsigned long long int _value ) {
  std::cout << unformatValue( _value ) << std::endl;
}

int main( int argc, char* argv[] ) {
  unsigned long long int kubicResult = kubic_main();

  std::cout << unformatValue( kubicResult ) << std::endl;

  return 0;
}