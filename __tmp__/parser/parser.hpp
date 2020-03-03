#ifndef _PARSER_HPP
#define _PARSER_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <queue>
#include <stack>

#include "tokenize.hpp"
#include "nodeify.hpp"
#include "../shared/token/Token.hpp"
#include "../shared/node/Node.hpp"

Node* parse( const std::string _entryFilename ) {
  std::ifstream entryFile( _entryFilename );

  if ( !entryFile.is_open() ) {
    /* TODO -- failed to open file */
    return nullptr;
  }

  std::stringstream entryFileContent;
  entryFileContent << entryFile.rdbuf();

  std::queue<Token> tokens = tokenize( entryFileContent.str() );
  Node* root = nodeify( tokens );

  if ( hasParserErrors() ) {
    printParserErrors();

    return nullptr;
  }

  return root;
}

#endif
