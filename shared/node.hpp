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

std::string formatValue( const NodeType _nodeType, const Token _token ) {
  int convertedValue;
  std::string value = _token.getText();
  std::string frontBase = value.substr( 0, 2 );

  switch ( _nodeType ) {
    case NodeType::NodeBoolean:
      if ( equals( value, "false" ) ) {
        return "0x7FFFFFFFFFFFFFFF";
      } else {
        return "0xFFFFFFFFFFFFFFFF";
      }
      break;
    case NodeType::NodeInteger:
      if ( equals( frontBase, "0x" ) ) {
        convertedValue = std::stoi( frontBase.substr( 2 ), nullptr, 16 );
      } else if ( equals( frontBase, "0o" ) ) {
        convertedValue = std::stoi( frontBase.substr( 2 ), nullptr, 8 );
      } else if ( equals( frontBase, "0b" ) ) {
        convertedValue = std::stoi( frontBase.substr( 2 ), nullptr, 2 );
      } else {
        convertedValue = std::stoi( frontBase );
      }

      convertedValue = convertedValue << 1;

      return std::to_string( convertedValue );
      break;
    case NodeType::NodeString:
      /* TODO -- add support for strings */
      return "";
      break;
    default:
      /* this should never be reached, otherwise it was called by mistake! */
      return "BEEFDADA";
      break;
  }
}

class BooleanNode : public Node {
  protected:
    Token boolean;

  public:
    BooleanNode( const Token _token ) : boolean( _token ) {
      nodeType = NodeType::NodeBoolean;
    }

    ~BooleanNode() {}

    #pragma GCC diagnostic ignored "-Wunused-parameter"
    std::string compile( Environment& _environment, unsigned int _offset = stackOffset ) const {
      return moveReg( Register::RAX, formatValue( nodeType, boolean ) );
    }
};

class IntegerNode : public Node {
  protected:
    Token integer;

  public:
    IntegerNode( const Token _token ) : integer( _token ) {
      nodeType = NodeType::NodeInteger;
    }

    ~IntegerNode() {}

    #pragma GCC diagnostic ignored "-Wunused-parameter"
    std::string compile( Environment& _environment, unsigned int _offset = stackOffset ) const {
      return moveReg( Register::RAX, formatValue( nodeType, integer ) );
    }
};

class VariableNode : public Node {
  protected:
    Token variable;

  public:
    VariableNode( const Token _token ) : variable( _token ) {
      nodeType = NodeType::NodeVariable;
    }

    ~VariableNode() {}

    #pragma GCC diagnostic ignored "-Wunused-parameter"
    std::string compile( Environment& _environment, unsigned int _offset = stackOffset ) const {
      return moveReg( Register::RAX, regOffset( RSI, searchBinding( _environment, variable ) ) );
    }
};

class BindingNode : public Node {
  protected:
    Token variable;
    Node* bindingExpression;

  public:
    BindingNode( const Token _variable, Node* _bindingExpression )
      : variable( _variable ), bindingExpression( _bindingExpression ) {
      nodeType = NodeType::NodeBinding;
    }

    ~BindingNode() {
      if ( bindingExpression ) delete bindingExpression;
    }

    std::string compile( Environment& _environment, unsigned int _offset = stackOffset ) const {
      *representation << bindingExpression->compile( _environment );
      *representation << moveReg( regOffset( Register::RSI, _offset ), Register::RAX );
      insertBinding( _environment, variable );
      return representation->str();
    }
};

class UnaryOperatorNode : public Node {
  protected:
    Token uOperator;
    Node* operand;

  public:
    UnaryOperatorNode( const Token _uOperator, Node* _operand ) :
      uOperator( _uOperator ), operand( _operand ) {
      nodeType = NodeType::NodeUnaryOperator;
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
    BinaryOperatorNode( const Token _bOperator, Node* _lOperand, Node* _rOperand )
      : bOperator( _bOperator ), lOperand( _lOperand ), rOperand( _rOperand ) {
      nodeType = NodeType::NodeBinaryOperator;
    }

    ~BinaryOperatorNode() {
      if ( lOperand ) delete lOperand;
      if ( rOperand ) delete rOperand;
    }

    std::string compile( Environment& _environment, unsigned int _offset = stackOffset ) const {
      *representation << rOperand->compile( _environment );
      *representation << moveReg( regOffset( Register::RSI, _offset ), Register::RAX );
      *representation << lOperand->compile( _environment, _offset + 1 );

      std::string binaryOperator;

      if ( equals( bOperator.getText(), "+" ) ) {
        binaryOperator = "add";
      } else if ( equals( bOperator.getText(), "-" ) ) {
        binaryOperator = "sub";
      } else if ( equals( bOperator.getText(), "*" ) ) {
        binaryOperator = "imul";
        *representation << operateReg( "sar", Register::RAX, "1" );
      } else if ( equals( bOperator.getText(), "/" ) ) {
        binaryOperator = "idiv";
      }

      *representation << operateReg( binaryOperator, Register::RAX, regOffset( Register::RSI, _offset ) );

      return representation->str();
    }
};

#endif
