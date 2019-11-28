#include <iostream>
#include <vector>

#include "headers/tokenizer/tokenizer.hpp"

int main() {
  std::string inputString = "let _foo = \r\r\rbar in _foo + 100";
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
