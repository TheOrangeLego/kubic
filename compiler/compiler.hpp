#ifndef _COMPILER_HPP
#define _COMPILER_HPP

#include <fstream>
#include <queue>
#include <string>

#include "shared/node.hpp"

std::string compileStatements( std::queue<Node*> _statements ) {
  std::string compilationResult = "";

  Environment environment;

  unsigned int stackOffset = 0;

  while ( !_statements.empty() ) {
    Node* statement = _statements.front();
    _statements.pop();
    
    compilationResult += statement->compile( environment, stackOffset++ );

    delete statement;
  }

  return compilationResult;
}

void compile( std::queue<Node*> _statements, const std::string _filename ) {
  std::string generatedCode = compileStatements( _statements );

  std::string asmFilename = _filename + ".ka";

  std::ofstream asmFile( asmFilename );

  asmFile << "section .text" << std::endl
          << "  global kubic_main" << std::endl
          << "kubic_main:" << std::endl
          << generatedCode
          << "  ret" << std::endl;
  asmFile.close();
}

#endif