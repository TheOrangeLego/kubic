#ifndef _TREEIFY_HPP
#define _TREEIFY_HPP

#include <vector>
#include <string>
#include "../token/Token.hpp"
#include "../tree/Tree.hpp"

TreeNode* treeify( std::vector<Token>& _tokens ) {
  TreeNode* node = nullptr;

  if ( !_tokens.empty() ) {
    if ( _tokens.at( 0 ).getType() == TokenType::Constant ) {
      node = new ConstNode( _tokens.at( 0 ) );
    }
  }

  return node;
}

#endif
