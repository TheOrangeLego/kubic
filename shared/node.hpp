#ifndef _NODE_HPP
#define _NODE_HPP

#include <sstream>
#include <string>
#include <vector>

#include "./types.hpp"

typedef std::pair<Token, unsigned int> EnvironmentBinding;

typedef std::vector<EnvironmentBinding> EnvironmentBindings;

class Node {
  protected:
    std::stringstream* representation;
    NodeType nodeType;

  public:
    Node() {
      representation = new std::stringstream();
    }

    virtual ~Node() {
      if ( representation ) delete representation;
    }

    virtual NodeType getType() const {
      return nodeType;
    }

    virtual std::string compile( const unsigned int _stackOffset, EnvironmentBindings& _bindings ) const = 0;
};

class ConstantNode : public Node {
  protected:
    Token constant;

  public:
    ConstantNode( const Token _token ) : constant( _token ) {
      nodeType = NodeType::ConstantNode;
    }

    ~ConstantNode() {}

    std::string compile( const unsigned int _stackOffset, EnvironmentBindings& _bindings ) const;
};

class VariableNode : public Node {
  protected:
    Token variable;

  public:
    VariableNode( const Token _token ) : variable( _token ) {
      nodeType = NodeType::VariableNode;
    }

    ~VariableNode() {}

    std::string compile( const unsigned int _stackOffset, EnvironmentBindings& _bindings ) const;
};

class UnaryOperatorNode : public Node {
  protected:
    Token uOperator;
    Node* operand;

  public:
    UnaryOperatorNode( const Token _uOperator, Node* _operand ) :
      uOperator( _uOperator ), operand( _operand ) {
      nodeType = NodeType::UnaryOperatorNode;
    }

    ~UnaryOperatorNode() {
      if ( operand ) delete operand;
    }

    std::string compile( const unsigned int _stackOffset, EnvironmentBindings& _bindings ) const;
};

class BinaryOperatorNode : public Node {
  protected:
    Token bOperator;
    Node* lOperand;
    Node* rOperand;

  public:
    BinaryOperatorNode( const Token _bOperator, Node* _lOperand, Node* _rOperand ) :
      bOperator( _bOperator ), lOperand( _lOperand ), rOperand( _rOperand ) {
      nodeType = NodeType::BinaryOperatorNode;
    }

    ~BinaryOperatorNode() {
      if ( lOperand ) delete lOperand;
      if ( rOperand ) delete rOperand;
    }

    std::string compile( const unsigned int _stackOffset, EnvironmentBindings& _bindings ) const;
};

class BindingNode : public Node {
  protected:
    Token variable;
    DataType dataType;
    Node* bindingExpression;

  public:
    BindingNode( const Token _variable, DataType _dataType, Node* _bindingExpression ) :
      variable( _variable ), dataType( _dataType ), bindingExpression( _bindingExpression ) {
      nodeType = NodeType::BindingNode;
    }

    ~BindingNode() {
      if ( bindingExpression ) delete bindingExpression;
    }

    std::string compile( const unsigned int _stackOffset, EnvironmentBindings& _bindings ) const;
};

#endif
