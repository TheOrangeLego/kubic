#ifndef _TREEIFY_HPP
#define _TREEIFY_HPP

#include <vector>
#include "../token/Token.hpp"
#include "../tree/Tree.hpp"

const std::string KEYWORDS[] = {
  "let",
  "in"
};

bool isNumeric( const std::string _token ) {
  for ( int index = 0; index < _token.length(); index++ ) {
    char tokenChar = _token[index];

    if ( ( tokenChar < 'a' || tokenChar > 'f' ) &&
         ( tokenChar < 'A' || tokenChar > 'F' ) &&
         ( tokenChar < '0' || tokenChar > '9' ) ) {
      return false;
    }
  }

  return true;
}

bool isKeyword( const std::string _token ) {
  for ( std::string keyword : KEYWORDS ) {
    if ( !keyword.compare( _token ) ) {
      return true;
    }
  }

  return false;
}

TokenNode* generateNode( const Token _token ) {
  TokenNode* node = nullptr;

  if ( isNumeric( _token ) ) {
    return node = new ConstNode( _token );
  }

  return node;
}

TokenNode* treeify( std::vector<Token>& _tokens ) {
  if ( _tokens.size() ) {
    
  } else {
    return nullptr;
  }
}

#endif
