#ifndef _COMPILER_HPP
#define _COMPILER_HPP

#include <fstream>
#include <queue>
#include <string>

#include "shared/node.hpp"

std::string compileStatements( std::queue<Node*> _statements ) {
  std::string compilationResult = "";

  Environment environment;

  while ( !_statements.empty() ) {
    Node* statement = _statements.front();
    _statements.pop();

    compilationResult += statement->compile( environment );

    delete statement;
  }

  return compilationResult;
}

bool compile( std::queue<Node*> _statements, const std::string _filename ) {
  Environment environment;

  std::string asmFilename = _filename + ".ka";

  std::ofstream asmFile( asmFilename );

  asmFile << "section .text" << std::endl
          << "  global kubic_main" << std::endl
          << "kubic_main:" << std::endl
          << compileStatements( _statements )
          << "  ret" << std::endl
          << std::endl;
  asmFile.close();

  return true;
}

#endif
