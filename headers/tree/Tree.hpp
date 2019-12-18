#ifndef _TREE_HPP
#define _TREE_HPP

#include <string>
#include <sstream>
#include "../token/Token.hpp"

class TreeNode {
  protected:
    std::stringstream* representation;

  public:
    TreeNode() {
      representation = new std::stringstream();
    }

    virtual ~TreeNode() {
      if ( representation ) delete representation;
    }

    virtual std::string print() const = 0;
};

class ConstNode : public TreeNode {
  protected:
    Token constantNode;
  
  public:
    ConstNode( const Token _token ) : constantNode( _token ) {}

    ~ConstNode() {}

    std::string print() const;
};

class BinaryOperator : public TreeNode {
  protected:
    TreeNode* leftNode;
    TreeNode* rightNode;
    std::string op;

  public:
    BinaryOperator( TreeNode* _lNode, TreeNode* _rNode, const std::string _op ) :
      leftNode( _lNode ), rightNode( _rNode ), op( _op ) {}

    ~BinaryOperator() {
      if ( leftNode ) delete leftNode;
      if ( rightNode ) delete rightNode;
    }

    std::string print() const;
};

#endif
