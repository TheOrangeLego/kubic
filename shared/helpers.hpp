#ifndef _HELPERS_HPP
#define _HELPERS_HPP

#include <string>

#include "token/Token.hpp"

inline bool contains( const std::string _string, const char _char ) {
  return _string.find( _char ) != std::string::npos;
}

inline bool equals( const std::string _stringA, const std::string _stringB ) {
  return !_stringA.compare( _stringB );
}

inline bool equals( const Token _tokenA, const Token _tokenB ) {
  return equals( _tokenA.getToken(), _tokenB.getToken() );
}

#endif
