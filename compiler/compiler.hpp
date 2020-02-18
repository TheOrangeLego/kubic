#ifndef _COMPILER_HPP
#define _COMPILER_HPP

#include <string>
#include <fstream>
#include <boost/format.hpp>

#include "../shared/assembly.hpp"
#include "../shared/messages.hpp"
#include "../shared/node/Node.hpp"

void compile( std::string _outputFilename, const Node* _root ) {
  std::ofstream generatedFile( _outputFilename );

  unsigned int stackOffset = 0;

  EnvironmentMap bindings;

  if ( generatedFile.is_open() ) {
    generatedFile << "section .text" << std::endl
                  << "  global kubic_main" << std::endl << std::endl
                  << "kubic_main:" << std::endl
                  << _root->compile( 0, bindings )
                  << "  ret" << std::endl;
    generatedFile.close();
  }
}

#endif
