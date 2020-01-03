#include <iostream>

extern "C" int kubic_main( void );

void error( unsigned int _errorNumber ) {
  exit( 1 );
}

int main( void ) {
  int result = kubic_main();

  std::cout << " > kubic yielded " << result << std::endl;

  return 0;
}
