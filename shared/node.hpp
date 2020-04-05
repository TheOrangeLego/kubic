#ifndef _NODE_HPP
#define _NODE_HPP

#include <sstream>
#include <string>
#include <vector>

#include "compiler/assembly.hpp"
#include "shared/helpers.hpp"
#include "shared/token.hpp"
#include "shared/types.hpp"

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
      nodeType = NodeType::ConstantNodeType;
    }

    ~ConstantNode() {}

    #pragma GCC diagnostic ignored "-Wunused-parameter"
    std::string compile( const unsigned int _stackOffset, EnvironmentBindings& _bindings) const {
      return printSingle( "  mov qword rax, %1%\n", constant.getText() );
    }
};

class VariableNode : public Node {
  protected:
    Token variable;

  public:
    VariableNode( const Token _token ) : variable( _token ) {
      nodeType = NodeType::VariableNodeType;
    }

    ~VariableNode() {}

    #pragma GCC diagnostic ignored "-Wunused-parameter"
    std::string compile( const unsigned int _stackOffset, EnvironmentBindings& _bindings ) const {
      unsigned int bindingsSize = _bindings.size();
      for ( unsigned int index = 0; index < bindingsSize; index++ ) {
        if ( equals( _bindings[bindingsSize - index - 1].first, variable ) ) {
          return printSingle( "  mov rax, %1%\n", regOffset( RSI, _bindings[index].second ) );
        }
      }
    }
};

class UnaryOperatorNode : public Node {
  protected:
    Token uOperator;
    Node* operand;

  public:
    UnaryOperatorNode( const Token _uOperator, Node* _operand ) :
      uOperator( _uOperator ), operand( _operand ) {
      nodeType = NodeType::UnaryOperatorNodeType;
    }

    ~UnaryOperatorNode() {
      if ( operand ) delete operand;
    }

    std::string compile( const unsigned int _stackOffset, EnvironmentBindings& _bindings ) const {
      *representation << operand->compile( _stackOffset, _bindings );

      return representation->str();
    }
};

class BinaryOperatorNode : public Node {
  protected:
    Token bOperator;
    Node* lOperand;
    Node* rOperand;

  public:
    BinaryOperatorNode( const Token _bOperator, Node* _lOperand, Node* _rOperand ) :
      bOperator( _bOperator ), lOperand( _lOperand ), rOperand( _rOperand ) {
      nodeType = NodeType::BinaryOperatorNodeType;
    }

    ~BinaryOperatorNode() {
      if ( lOperand ) delete lOperand;
      if ( rOperand ) delete rOperand;
    }

    std::string compile( const unsigned int _stackOffset, EnvironmentBindings& _bindings ) const {
      *representation << rOperand->compile( _stackOffset, _bindings );
      formatSingle( representation, "  mov %1%, rax\n", regOffset( RSI, _stackOffset ) );
      *representation << lOperand->compile( _stackOffset + 1, _bindings );

      if ( equals( bOperator.getText(), "+" ) ) {
        formatSingle( representation, "  add rax, %1%\n", regOffset( RSI, _stackOffset ) );
      } else if ( equals( bOperator.getText(), "-" ) ) {
        formatSingle( representation, "  sub rax, %1%\n", regOffset( RSI, _stackOffset ) );
      } else if ( equals( bOperator.getText(), "*" ) ) {
        formatSingle( representation, "  imul rax, %1%\n", regOffset( RSI, _stackOffset ) );
      } else if ( equals( bOperator.getText(), "/" ) ) {
        formatSingle( representation, "  idiv rax, %1%\n", regOffset( RSI, _stackOffset ) );
      }

      return representation->str();
    }
};

class BindingNode : public Node {
  protected:
    Token variable;
    DataType dataType;
    Node* bindingExpression;

  public:
    BindingNode( const Token _variable, DataType _dataType, Node* _bindingExpression ) :
      variable( _variable ), dataType( _dataType ), bindingExpression( _bindingExpression ) {
      nodeType = NodeType::BindingNodeType;
    }

    ~BindingNode() {
      if ( bindingExpression ) delete bindingExpression;
    }

    std::string compile( const unsigned int _stackOffset, EnvironmentBindings& _bindings ) const {
      /* TODO -- implement binding with types */

      return representation->str();
    }
};

#endif
