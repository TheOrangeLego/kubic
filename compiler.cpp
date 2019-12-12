#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "headers/tokenizer/Tokenizer.hpp"
#include "headers/treeify/Treeify.hpp"

const std::string ASM_KUBIC = "main.asm";

int main( int argc, char* argv[] ) {
  std::string inputString;

  if ( argc > 1 ) {
    std::ifstream inputFile( argv[1], std::ifstream::in );
    std::stringstream inputBuffer;

    inputBuffer << inputFile.rdbuf();
    inputString = inputBuffer.str();
  } else {
    inputString = "1010";
  }

  std::vector<Token> tokens = tokenize( inputString );
  TreeNode* root = treeify( tokens );

  std::ofstream kubicASM( ASM_KUBIC );

  if ( kubicASM.is_open() ) {
    kubicASM << "section .text" << std::endl;
    kubicASM << "  global kubic_main" << std::endl;
    kubicASM << "kubic_main:" << std::endl;
    kubicASM << root->print();
    kubicASM << "  ret" << std::endl;
    kubicASM.close();
  }

  return 0;
}
