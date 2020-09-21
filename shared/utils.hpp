#ifndef _UTILS_HPP
#define _UTILS_HPP

#include <set>

template<class T>
bool contains( const std::set<T> _set, const T _element ) {
  return _set.find( _element ) != _set.end();
}

template<class T, class K>
bool contains( const std::map<T, K> _map, const T _element ) {
  return _map.find( _element ) != _map.end();
}

template<class T, class K>
K mapping( const std::map<T, K> _map, const T _element, const K _default ) {
  if ( _map.find( _element ) == _map.end() ) {
    return _default;
  } else {
    return _map.at( _element );
  }
} 

#endif
