#include <iostream>

extern "C" int kubic_main( void );

void error( unsigned int _errorCode ) {
  exit( 1 );
}

int main( void ) {
  int kubicResult = kubic_main();

  // if ( kubicResult ) error( kubicResult );

  std::cout << "Kubic returned :: " << kubicResult << std::endl;

  return 0;
}
