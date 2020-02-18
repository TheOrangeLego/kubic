#ifndef _NODE_HPP
#define _NODE_HPP

#include <string>
#include <sstream>
#include <vector>

#include "../token/Token.hpp"

typedef std::pair<Token, unsigned int> EnvironmentPair;
typedef std::vector<EnvironmentPair> EnvironmentMap;

class Node {
  protected:
    std::stringstream* representation;

  public:
    Node() { representation = new std::stringstream(); }

    virtual ~Node() {
      if ( representation ) delete representation;
    }

    virtual std::string compile( const unsigned int _stackOffset, EnvironmentMap& _bindings ) const = 0;
};

class ConstantNode : public Node {
  protected:
    Token constant;
  
  public:
    ConstantNode( const Token _token ) : constant( _token ) {}

    ~ConstantNode() {}

    std::string compile( const unsigned int _stackOffset, EnvironmentMap& _bindings ) const;
};

class VariableNode : public Node {
  protected:
    Token variable;
  
  public:
    VariableNode( const Token _token ) : variable( _token ) {}

    ~VariableNode() {}

    std::string compile( const unsigned int _stackOffset, EnvironmentMap& _bindings ) const;
};

class UnaryOperatorNode : public Node {
  protected:
    Token operate;
    Node* operand;
  
  public:
    UnaryOperatorNode( const Token _token, Node* _operand ) : operate( _token ), operand( _operand ) {}

    ~UnaryOperatorNode() {
      if ( operand ) delete operand;
    }

    std::string compile( const unsigned int _stackOffset, EnvironmentMap& _bindings ) const;
};

class BinaryOperatorNode : public Node {
  protected:
    Token operate;
    Node* lOperand;
    Node* rOperand;

  public:
    BinaryOperatorNode( const Token _token, Node* _lOperand, Node* _rOperand ) :
      operate( _token ), lOperand( _lOperand ), rOperand( _rOperand ) {}
    
    ~BinaryOperatorNode() {
      if ( lOperand ) delete lOperand;
      if ( rOperand ) delete rOperand;
    }

    std::string compile( const unsigned int _stackOffset, EnvironmentMap& _bindings ) const;
};

class BindingNode : public Node {
  protected:
    Token variable;
    Node* binding;
    Node* body;

  public:
    BindingNode( const Token _variable, Node* _binding, Node* _body ) :
      variable( _variable ), binding( _binding ), body( _body ) {}
    
    ~BindingNode() {
      if ( binding ) delete binding;
      if ( body ) delete body;
    }

    std::string compile( const unsigned int _stackOffset, EnvironmentMap& _bindings ) const;
};

#endif
