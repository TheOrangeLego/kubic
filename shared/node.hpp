#ifndef _NODE_HPP
#define _NODE_HPP

#include <sstream>
#include <string>
#include <vector>

#include "compiler/assembly.hpp"
#include "shared/helpers.hpp"
#include "shared/token.hpp"
#include "shared/types.hpp"

static unsigned int stackOffset = 0;

typedef std::pair<Token, unsigned int> EnvironmentBinding;

typedef std::vector<EnvironmentBinding> Environment;

void insertBinding( Environment& _environment, Token _token ) {
  _environment.push_back( EnvironmentBinding( _token, stackOffset ) );
  stackOffset++;
}

unsigned int searchBinding( Environment& _environment, Token _token ) {
  for ( EnvironmentBinding binding : _environment ) {
    if ( equals( binding.first, _token ) ) {
      return binding.second;
    }
  }

  return 0;
}

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

    virtual std::string compile(
      Environment& _environment, unsigned int _offset = stackOffset
    ) const = 0;
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
    std::string compile( Environment& _environment, unsigned int _offset = stackOffset ) const {
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
    std::string compile( Environment& _environment, unsigned int _offset = stackOffset ) const {
      return printSingle( "  mov rax, %1%\n", regOffset( RSI, searchBinding( _environment, variable ) ) );
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

    std::string compile( Environment& _environment, unsigned int _offset = stackOffset ) const {
      *representation << operand->compile( _environment );

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

    std::string compile( Environment& _environment, unsigned int _offset = stackOffset ) const {
      *representation << rOperand->compile( _environment );
      formatSingle( representation, "  mov %1%, rax\n", regOffset( RSI, _offset ) );
      *representation << lOperand->compile( _environment, _offset + 1 );

      if ( equals( bOperator.getText(), "+" ) ) {
        formatSingle( representation, "  add rax, %1%\n", regOffset( RSI, _offset ) );
      } else if ( equals( bOperator.getText(), "-" ) ) {
        formatSingle( representation, "  sub rax, %1%\n", regOffset( RSI, _offset ) );
      } else if ( equals( bOperator.getText(), "*" ) ) {
        formatSingle( representation, "  imul rax, %1%\n", regOffset( RSI, _offset ) );
      } else if ( equals( bOperator.getText(), "/" ) ) {
        formatSingle( representation, "  idiv rax, %1%\n", regOffset( RSI, _offset ) );
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

    std::string compile( Environment& _environment, unsigned int _offset = stackOffset ) const {
      *representation << bindingExpression->compile( _environment );
      formatSingle( representation, "  mov %1%, rax\n", regOffset( RSI, _offset ) );
      insertBinding( _environment, variable );
      return representation->str();
    }
};

#endif
