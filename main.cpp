#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include "headers/tokenizer/tokenizer.hpp"

int main( int argc, char* argv[] ) {
  std::string inputString;

  if ( argc > 1 ) {
    std::ifstream inputFile( argv[1], std::ifstream::in );
    std::stringstream inputBuffer;

    inputBuffer << inputFile.rdbuf();
    inputString = inputBuffer.str();
  } else {
    inputString = "let _foo = \rbar in _foo + 100";
  }

  std::vector<Token> tokens = tokenize( inputString );
  std::cout << inputString << std::endl;

  for ( Token token : tokens ) {
    std::cout << "Token ::" << std::endl;
    std::cout << '\t' << token.getToken() << std::endl;
    std::cout << "\t(" << token.getPositionLine() << ", " << token.getPositionCol() << ")" << std::endl;
    std::cout << std::endl;
  }

  return 0;
}
