#ifndef _NODE_HPP
#define _NODE_HPP

#include <sstream>
#include <string>
#include <vector>

#include "compiler/assembly.hpp"
#include "shared/environment.hpp"
#include "shared/error.hpp"
#include "shared/helpers.hpp"
#include "shared/token.hpp"
#include "shared/types.hpp"

std::string formatValue( const NodeType _nodeType, const Token _token ) {
  int convertedValue;
  std::string value = _token.getText();
  std::string frontBase = value.substr( 0, 2 );

  switch ( _nodeType ) {
    case NodeType::NodeBoolean:
      if (  value == "false" ) {
        return "0x7FFFFFFFFFFFFFFF";
      } else {
        return "0xFFFFFFFFFFFFFFFF";
      }
      break;
    case NodeType::NodeInteger:
      if ( frontBase == "0x" ) {
        convertedValue = std::stoi( value.substr( 2 ), nullptr, 16 );
      } else if ( frontBase == "0o" ) {
        convertedValue = std::stoi( value.substr( 2 ), nullptr, 8 );
      } else if ( frontBase == "0b" ) {
        convertedValue = std::stoi( value.substr( 2 ), nullptr, 2 );
      } else {
        convertedValue = std::stoi( value );
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

  return "";
}

class Node {
  protected:
    std::stringstream* representation;
    NodeType nodeType;

  public:
    Node() {
      representation = new std::stringstream();
      nodeType = NodeType::NodeUndefined;
    }

    virtual ~Node() {
      if ( representation ) delete representation;
    }

    virtual NodeType getNodeType() const {
      return nodeType;
    }

    virtual std::string compile( Environment& _environment, ErrorLogger& _errorLogger ) = 0;
};

bool operator == ( const Node* _node, const NodeType _type ) {
  return _node->getNodeType() == _type;
} 

template<typename... Arguments>
void ErrorLogger::logError( boost::format& _message, Node* _node, Arguments... _arguments ) {
  logError( _message % typeToString( _node->getNodeType() ), _arguments... );
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
    std::string compile( Environment& _environment, ErrorLogger& _errorLogger ) {
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
    std::string compile( Environment& _environment, ErrorLogger& _errorLogger ) {
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
    std::string compile( Environment& _environment, ErrorLogger& _errorLogger ) {
      if ( !_environment.bindingExists( variable ) ) {
        _errorLogger.logError( ERR_UNDEFINED_VARIABLE, variable );
        return "";
      }

      nodeType = _environment.getType( variable );
      return moveReg( Register::RAX, regOffset( RSI, _environment.getOffset( variable ) ) );
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
    }

    ~BindingNode() {
      if ( bindingExpression ) delete bindingExpression;
    }

    std::string compile( Environment& _environment, ErrorLogger& _errorLogger ) {
      *representation << bindingExpression->compile( _environment, _errorLogger );
      *representation << moveReg( regOffset( Register::RSI, _environment.currentOffset() ), Register::RAX );

      if ( bindingExpression == NodeType::NodeUndefined ) {
        _errorLogger.logError( ERR_BINDING_BODY_TYPE_INVALID );
        return "";
      } else if ( _environment.bindingExists( variable ) ) {
        _errorLogger.logError( ERR_BINDING_ALREADY_EXISTS, variable );
        return "";
      } else if ( stringToType( typeBinded.getText() ) != bindingExpression->getNodeType() ) {
        _errorLogger.logError( ERR_BINDING_BODY_TYPE_MISMATCH, typeBinded, bindingExpression );
        return "";
      }

      _environment.insertBinding( variable, bindingExpression->getNodeType() );

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

    std::string compile( Environment& _environment, ErrorLogger& _errorLogger ) {
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
    }

    ~BinaryOperatorNode() {
      if ( lOperand ) delete lOperand;
      if ( rOperand ) delete rOperand;
    }

    std::string compile( Environment& _environment, ErrorLogger& _errorLogger ) {
      unsigned int currentOffset = _environment.currentOffset();
      *representation << rOperand->compile( _environment, _errorLogger );
      *representation << moveReg( regOffset( Register::RSI, currentOffset ), Register::RAX );
      *representation << lOperand->compile( _environment, _errorLogger );

      NodeType lNodeType = lOperand->getNodeType();
      NodeType rNodeType = rOperand->getNodeType();
      BinaryTypeInference inferedTypeMap( lNodeType, bOperator.getText(), rNodeType );

      if ( contains( BINARY_OPERATOR_INFERED_TYPES, inferedTypeMap ) ) {
        nodeType = BINARY_OPERATOR_INFERED_TYPES.at( inferedTypeMap );
      } else {
        _errorLogger.logError( ERR_BINARY_OPERATOR_TYPE_MISMATCH, bOperator, lOperand, rOperand );
        return "";
      }

      std::string binaryOperator;

      /* might be able to do without this if we combine assembly instructions with BINARY_OPERATOR_INFERED_TYPES */
      if ( lOperand == NodeType::NodeInteger &&  rOperand == NodeType::NodeInteger ) {
        if ( bOperator == "+" ) {
          binaryOperator = "add";
        } else if ( bOperator == "-" ) {
          binaryOperator = "sub";
        } else if ( bOperator == "*" ) {
          binaryOperator = "imul";
          *representation << operateReg( "sar", Register::RAX, "1" );
        } else if ( bOperator == "/" ) {
          binaryOperator = "idiv";
        }
      }

      *representation << operateReg( binaryOperator, Register::RAX, regOffset( Register::RSI, currentOffset ) );

      return representation->str();
    }
};

#endif
