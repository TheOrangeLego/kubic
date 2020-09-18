#include <fstream>
#include <iostream>
#include <string>

#include "compiler/compiler.hpp"
#include "parser/parser.hpp"
#include "shared/errors.hpp"

int main( int argc, char* argv[] ) {
  if ( argc == 1 ) {
    /* print info and usage message */
    return 1;
  } else {
    Node* root = parse( argv[1] );

    if ( !root ) {
      printErrors();

      return 10;
    }

    compile( root, "main" );
  }

  return 0;
}