#ifndef _EXECUTER_HPP
#define _EXECUTER_HPP

#include <iostream>

extern "C" int kubic_main( void );

void error( unsigned int _errorNumber ) {
  exit( 1 );
}

void execute( void ) {
  int kubicResult = kubic_main();

  if ( kubicResult ) error( kubicResult );
}

#endif
