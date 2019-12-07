#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "headers/tokenizer/tokenizer.hpp"

const std::string ASM_KUBIC = "_kubic.asm";

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
  std::ofstream kubicASM( ASM_KUBIC );

  if ( kubicASM.is_open() ) {
    kubicASM << "section .text" << std::endl;
    kubicASM << "  global kubic_main" << std::endl;
    kubicASM << "kubic_main:" << std::endl;
    kubicASM << "  mov rax, 1010" << std::endl;
    kubicASM << "  ret" << std::endl;
    kubicASM.close();
  }

  return 0;
}
