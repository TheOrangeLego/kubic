#include <iostream>
#include <string>

#include "parser/parser.hpp"
#include "compiler/compiler.hpp"
#include "executer/executer.hpp"

int main( int argc, char* argv[] ) {
  std::string entryFilename = "main.kbc";
  std::string outputFilename = "main.asm";

  Node* parsingResult = parse( entryFilename );
  compile( outputFilename, parsingResult );

  if ( false ) {
    execute();
  }

  return 0;
}
