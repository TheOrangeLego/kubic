#ifndef _HELPERS_HPP
#define _HELPERS_HPP

#include <boost/format.hpp>
#include <set>
#include <string>
#include <sstream>

#include "shared/token.hpp"

inline bool contains( const std::string _string, const char _char ) {
  return _string.find( _char ) != std::string::npos;
}

template <class T>
inline bool contains( const std::set<T> _set, const T _element ) {
  return _set.find( _element ) != _set.end();
}

inline bool equals( const std::string _stringA, const std::string _stringB ) {
  return !_stringA.compare( _stringB );
}

inline bool equals( const Token _tokenA, const Token _tokenB ) {
  return equals( _tokenA.getText(), _tokenB.getText() );
}

inline bool equals( const Token _token, const std::string _string ) {
  return equals( _token.getText(), _string );
}

inline void formatSingle( std::stringstream* _stream, std::string _string, std::string _variable ) {
  *_stream << boost::format( _string ) % _variable;
}

inline std::string printSingle( std::string _string, std::string _variable ) {
  return ( boost::format( _string ) % _variable ).str();
}

#endif
