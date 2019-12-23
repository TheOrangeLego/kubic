#include <iostream>

extern "C" int kubic_main( void );

int main( void ) {
  int result = kubic_main();

  std::cout << "Kubic yielded " << result << std::endl;

  return 0;
}
