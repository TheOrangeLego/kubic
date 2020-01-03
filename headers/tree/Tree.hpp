#ifndef _TREE_HPP
#define _TREE_HPP

#include <string>
#include <sstream>
#include <vector>
#include <boost/format.hpp>

#include "../token/Token.hpp"

typedef std::vector<std::pair<std::string, unsigned int>> EnvironmentMap;

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

    virtual std::string compile( const unsigned int _stackOffset, EnvironmentMap& _bindings ) const = 0;
};

class ConstantNode : public TreeNode {
  protected:
    Token constant;
  
  public:
    ConstantNode( const Token _token ) : constant( _token ) {}

    ~ConstantNode() {}

    std::string compile( const unsigned int _stackOffset, EnvironmentMap& _bindings ) const;
};

class VariableNode : public TreeNode {
  protected:
    Token variable;

  public:
    VariableNode( const Token _token ) : variable( _token ) {}

    ~VariableNode() {}

    std::string compile( const unsigned int _stackOffset, EnvironmentMap& _bindings ) const;
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

    std::string compile( const unsigned int _stackOffset, EnvironmentMap& _bindings ) const;
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

    std::string compile( const unsigned int _stackOffset, EnvironmentMap& _bindings ) const;
};

class BindingNode : public TreeNode {
  protected:
    Token variable;
    TreeNode* bindingNode;
    TreeNode* bodyNode;

  public:
    BindingNode( const Token _variable, TreeNode* _bindingNode, TreeNode* _bodyNode ) :
      variable( _variable ), bindingNode( _bindingNode ), bodyNode( _bodyNode ) {}

    ~BindingNode() {
      if ( bindingNode ) delete bindingNode;
      if ( bodyNode ) delete bodyNode;
    }

    std::string compile( const unsigned int _stackOffset, EnvironmentMap& _bindings ) const;
};

#endif
