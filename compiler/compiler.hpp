#ifndef _COMPILER_HPP
#define _COMPILER_HPP

#include <string>
#include <fstream>

#include "shared/node.hpp"

bool compile( const Node* _rootNode, const std::string _filename ) {
  unsigned int stackOffset = 0;

  EnvironmentBindings environmentBindings;

  std::string asmFilename = _filename + ".ka";

  std::ofstream asmFile( asmFilename );

  asmFile << "section .text" << std::endl
          << "  global kubic_main" << std::endl
          << "kubic_main:" << std::endl
          << _rootNode->compile( stackOffset, environmentBindings )
          << "  ret" << std::endl
          << std::endl;
  asmFile.close();

  return true;
}

#endif
