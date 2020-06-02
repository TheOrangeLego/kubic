#include <iostream>

const unsigned long long int FALSE = 0x7FFFFFFFFFFFFFFF;
const unsigned long long int TRUE  = 0xFFFFFFFFFFFFFFFF;

extern "C" unsigned long long int kubic_main( void );

void error( long long int _errorCode ) {
  exit( 1 );
}

std::string unformatValue( unsigned long long int _value ) {
  if ( _value == FALSE ) {
    return "false";
  } else if ( _value == TRUE ) {
    return "true";
  } else if ( ( _value & 0x1 ) == 0x0 ) {
    return std::to_string( _value >> 1 );
  }

  return "";
}
int main( void ) {
  unsigned long long int kubicResult = kubic_main();

  // if ( kubicResult ) error( kubicResult );

  std::cout << "Kubic returned :: " << unformatValue( kubicResult ) << std::endl;

  return 0;
}
