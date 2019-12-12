#ifndef _TREE_HPP
#define _TREE_HPP

#include <sstream>
#include "../token/Token.hpp"

class TreeNode {
  protected:
    Token token;
  public:
    TreeNode( const Token _token ) : token( _token ) {}

    std::string getTokenString() const;
    TokenType getTokenType() const;
    TokenPosition getTokenPosition() const;

    virtual std::string print() const = 0;
};

class ConstNode : public TreeNode {
  public:
    ConstNode( const Token _token ) : TreeNode( _token ) {}

    std::string print() const;
};

#endif
