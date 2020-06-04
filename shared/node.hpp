#ifndef _NODE_HPP
#define _NODE_HPP

#include <sstream>
#include <string>
#include <vector>

#include "compiler/assembly.hpp"
#include "shared/error.hpp"
#include "shared/helpers.hpp"
#include "shared/token.hpp"
#include "shared/types.hpp"

static unsigned int stackOffset = 0;

typedef std::tuple<Token, unsigned int, NodeType> EnvironmentBinding;

typedef std::vector<EnvironmentBinding> Environment;

void insertBinding( Environment& _environment, const Token _token, const NodeType _dataType ) {
  _environment.push_back( EnvironmentBinding( _token, stackOffset, _dataType ) );
  stackOffset++;
}

unsigned int getBindingOffset( Environment& _environment, const Token _token ) {
  for ( EnvironmentBinding binding : _environment ) {
    if ( equals( std::get<0>( binding ), _token ) ) {
      return std::get<1>( binding );
    }
  }
}

NodeType getBindingNodeType( Environment& _environment, const Token _token ) {
  for ( EnvironmentBinding binding : _environment ) {
    if ( equals( std::get<0>( binding ), _token ) ) {
      return std::get<2>( binding );
    }
  }
}

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

    virtual NodeType getNodeType() const {
      return nodeType;
    }

    virtual std::string compile(
      Environment& _environment, ErrorLogger& _errorLogger, unsigned int _offset = stackOffset
    ) const = 0;
};

bool matchesNodeType( const Node* _node, const NodeType _nodeType ) {
  return _node->getNodeType() == _nodeType;
}

bool matchesNodeType( const Node* _nodeA, const NodeType _nodeTypeA, const Node* _nodeB, const NodeType _nodeTypeB ) {
  return matchesNodeType( _nodeA, _nodeTypeA ) && matchesNodeType( _nodeB, _nodeTypeB );
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
    std::string compile(
      Environment& _environment, ErrorLogger& _errorLogger, unsigned int _offset = stackOffset
    ) const {
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
    std::string compile(
      Environment& _environment, ErrorLogger& _errorLogger, unsigned int _offset = stackOffset
    ) const {
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
    std::string compile(
      Environment& _environment, ErrorLogger& _errorLogger, unsigned int _offset = stackOffset
    ) const {
      return moveReg( Register::RAX, regOffset( RSI, getBindingOffset( _environment, variable ) ) );
    }
};

class BindingNode : public Node {
  protected:
    Token variable;
    Token typeBinded;
    Node* bindingExpression;

  public:
    BindingNode( const Token _variable, const Token _typeBinded, Node* _bindingExpression )
      : variable( _variable ), typeBinded( _typeBinded ), bindingExpression( _bindingExpression ) {
      nodeType = NodeType::NodeBinding;
    }

    ~BindingNode() {
      if ( bindingExpression ) delete bindingExpression;
    }

    std::string compile(
      Environment& _environment, ErrorLogger& _errorLogger, unsigned int _offset = stackOffset
    ) const {
      if (
        STRING_TO_NODE_TYPE.find( typeBinded.getText() ) != STRING_TO_NODE_TYPE.end()
        && STRING_TO_NODE_TYPE.at( typeBinded.getText() ) != bindingExpression->getNodeType()
      ) {
        /* type-mismatch between assigned type and infered body type */
      }

      *representation << bindingExpression->compile( _environment, _errorLogger );
      *representation << moveReg( regOffset( Register::RSI, _offset ), Register::RAX );
      insertBinding( _environment, variable, bindingExpression->getNodeType() );
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
    }

    ~UnaryOperatorNode() {
      if ( operand ) delete operand;
    }

    std::string compile(
      Environment& _environment, ErrorLogger& _errorLogger, unsigned int _offset = stackOffset
    ) const {
      *representation << operand->compile( _environment, _errorLogger );

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
      BinaryTypeInference typeInfered( _lOperand->getNodeType(), _bOperator.getText(), _rOperand->getNodeType() );

      if ( BINARY_OPERATOR_INFERED_TYPES.find( typeInfered ) == BINARY_OPERATOR_INFERED_TYPES.end() ) {
        /* TODO -- static type error */
      }

      nodeType = BINARY_OPERATOR_INFERED_TYPES.at(typeInfered);
    }

    ~BinaryOperatorNode() {
      if ( lOperand ) delete lOperand;
      if ( rOperand ) delete rOperand;
    }

    std::string compile(
      Environment& _environment, ErrorLogger& _errorLogger, unsigned int _offset = stackOffset
    ) const {
      *representation << rOperand->compile( _environment, _errorLogger );
      *representation << moveReg( regOffset( Register::RSI, _offset ), Register::RAX );
      *representation << lOperand->compile( _environment, _errorLogger, _offset + 1 );

      std::string binaryOperator;

      /* might be able to do without this if we combine assembly instructions with BINARY_OPERATOR_INFERED_TYPES */
      if ( matchesNodeType( lOperand, NodeType::NodeInteger, rOperand, NodeType::NodeInteger ) ) {
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
      }

      *representation << operateReg( binaryOperator, Register::RAX, regOffset( Register::RSI, _offset ) );

      return representation->str();
    }
};

#endif
