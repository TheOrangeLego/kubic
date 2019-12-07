#include <iostream>

extern "C" int kubic_main( void );

int main( void ) {
  std::cout << "Kubic yielded " << kubic_main() << std::endl;

  return 0;
}
