#ifndef _HELPERS_HPP
#define _HELPERS_HPP

#include <map>
#include <set>
#include <string>

inline bool contains( const std::string _string, const char _char ) {
  return _string.find( _char ) != std::string::npos;
}

template <class T>
inline bool contains( const std::set<T> _set, const T _element ) {
  return _set.find( _element ) != _set.end();
}

template<class T, class K>
inline bool contains( const std::map<T, K> _map, const T _element ) {
  return _map.find( _element ) != _map.end();
}

bool operator == ( const std::string _stringA, const std::string _stringB ) {
  return !_stringA.compare( _stringB );
}

bool operator != ( const std::string _stringA, const std::string _stringB ) {
  return !( _stringA == _stringB );
}

#endif
