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

    virtual std::string print( const unsigned int _stackOffset ) const = 0;
};

class ConstNode : public TreeNode {
  protected:
    Token constantNode;
  
  public:
    ConstNode( const Token _token ) : constantNode( _token ) {}

    ~ConstNode() {}

    std::string print( const unsigned int _stackOffset ) const;
};

class UnaryOperator : public TreeNode {
  protected:
    Token op;
    TreeNode* node;

  public:
    UnaryOperator( const Token _op, TreeNode* _node ) :
      op( _op ), node( _node ) {}
    
    ~UnaryOperator() {
      if ( node ) delete node;
    }

    std::string print( const unsigned int _stackOffset ) const;
};

class BinaryOperator : public TreeNode {
  protected:
    Token op;
    TreeNode* leftNode;
    TreeNode* rightNode;

  public:
    BinaryOperator( const Token _op, TreeNode* _lNode, TreeNode* _rNode ) :
      op( _op ), leftNode( _lNode ), rightNode( _rNode ) {}

    ~BinaryOperator() {
      if ( leftNode ) delete leftNode;
      if ( rightNode ) delete rightNode;
    }

    std::string print( const unsigned int _stackOffset ) const;
};

#endif
