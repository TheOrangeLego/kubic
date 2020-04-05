#include <fstream>
#include <iostream>
#include <string>

#include "compiler/compiler.hpp"
#include "parser/lexer.hpp"
#include "parser/parser.hpp"
#include "shared/messages.hpp"

int main( int argc, char* argv[] ) {
  if ( argc == 1 ) {
    std::cout << MSG_USAGE << std::endl;
    return 0;
  }
  /* parse for flags */

  /* will need a string that contains the name of file instead of relying on argv[1] */
  std::ifstream entryFile ( argv[1] );
  /* read entry file's contents */
  std::string entryFileContent(
    ( std::istreambuf_iterator<char>( entryFile ) ), ( std::istreambuf_iterator<char>() )
  );
  /* should extract filename from argument */
  std::queue<Token> tokens = tokenize( entryFileContent, argv[1] );

  Node* rootNode = parse( tokens );
  /* TODO -- allow dynamic file name once the Kubic driver is not generated through Makefile */
  bool successfulCompile = compile( rootNode, "main" );

  if ( !successfulCompile ) {
    /* show error messages */
    return -2;
  }

  return 0;
}
