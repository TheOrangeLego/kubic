#include <fstream>
#include <iostream>
#include <string>

#include "compiler/compiler.hpp"
#include "parser/lexer.hpp"
#include "parser/parser.hpp"
#include "shared/messages.hpp"

int main( int argc, char* argv[] ) {
  /* will need a string that contains the name of file instead of relying on argv[1] */
  std::ifstream entryFile ( argv[1] );

  /* read entry file's contents */
  std::string entryFileContent(
    ( std::istreambuf_iterator<char>( entryFile ) ), ( std::istreambuf_iterator<char>() )
  );

  /* should extract filename from argument */
  std::queue<Token> tokens = tokenize( entryFileContent, argv[1] );

  /* store each parsed statement */
  std::queue<Node*> statements = parse( tokens );

  /* TODO -- allow dynamic file name once the Kubic driver is not generated through Makefile */
  ( void ) compile( statements, "main" );

  return 0;
}
