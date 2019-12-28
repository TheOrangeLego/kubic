#include <queue>
#include <string>
#include <fstream>
#include <sstream>

#include "headers/tokenize.hpp"
#include "headers/treeify.hpp"

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

  std::queue<Token> tokens = tokenize( inputString );
  TreeNode* root = treeify( tokens );

  std::ofstream kubicASM( ASM_KUBIC );

  if ( kubicASM.is_open() ) {
    kubicASM << "section .text" << std::endl;
    kubicASM << "  global kubic_main" << std::endl << std::endl;
    kubicASM << "kubic_main:" << std::endl;
    kubicASM << root->print();
    kubicASM << "  ret" << std::endl;
    kubicASM.close();
  }

  if ( root ) delete root;

  return 0;
}
