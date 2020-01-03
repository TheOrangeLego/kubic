#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <boost/format.hpp>

#include "headers/helpers.hpp"
#include "headers/tokenize.hpp"
#include "headers/treeify.hpp"

const std::string ASM_KUBIC = "main.asm";

const std::string MSG_USAGE = "\
Usage:\n\
  kubic-c [--options] <KBC entry file>\n\
\n\
Options:\n\
  -v | --version\tReturn the compiler version\n\
  -h | --help\t\tPrints this message\n\
  -l | --log\tPrint debugging statements during compilation\
";

const std::string ERR_INVALID_OPTION = "\
Invalid option %1% provided. Use the -h | --help option to view all valid options.\
";

static std::string entryFile = "";
static bool flagInvalidOption = false;
static bool flagShowHelpMessage = false;
static bool flagShowVersion = false;
static bool flagVerbose = false;

void parseOptions( int _argc, char* _argv[] ) {
  for ( int index = 1; index < _argc; index++ ) {
    if ( equals( _argv[index], "-h" ) || equals( _argv[index], "--help" ) ) flagShowHelpMessage = true;
    else if ( equals( _argv[index], "-l" ) || equals( _argv[index], "--log" ) ) flagVerbose = true;
    else if ( equals( _argv[index], "-v" ) || equals( _argv[index], "--version" ) ) flagShowVersion = true;
  }
}

int main( int _argc, char* _argv[] ) {
  std::string inputString;

  if ( _argc < 2 ) {
    std::cout << MSG_USAGE << std::endl;

    return 0;
  }

  parseOptions( _argc, _argv );

  std::ifstream inputFile( _argv[1], std::ifstream::in );
  std::stringstream inputBuffer;

  inputBuffer << inputFile.rdbuf();
  inputString = inputBuffer.str();

  std::queue<Token> tokens = tokenize( inputString );
  TreeNode* root = treeify( tokens );
  EnvironmentMap bindings;

  std::ofstream kubicASM( ASM_KUBIC );

  if ( kubicASM.is_open() ) {
    kubicASM << "section .text" << std::endl;
    kubicASM << "  global kubic_main" << std::endl << std::endl;
    kubicASM << "kubic_main:" << std::endl;
    kubicASM << root->compile( 0, bindings );
    kubicASM << "  ret" << std::endl;
    kubicASM.close();
  }

  if ( root ) delete root;

  return 0;
}
