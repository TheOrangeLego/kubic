#ifndef _NODE_HPP
#define _NODE_HPP

#include <sstream>
#include <string>
#include <tuple>

#include "shared/environment.hpp"
#include "shared/errors.hpp"
#include "shared/position.hpp"
#include "shared/types.hpp"
#include "shared/utils.hpp"

typedef std::tuple<std::string, ValueType> UnaryOperatorValue;

typedef std::tuple<std::string, ValueType, ValueType> BinaryOperatorValue;

const std::map<BinaryOperatorValue, ValueType> DERIVED_BINARY_VALUES = {
  /* derived boolean binary values */
  { BinaryOperatorValue( "&&", ValueType::ValueBoolean, ValueType::ValueBoolean ), ValueType::ValueBoolean },
  { BinaryOperatorValue( "||", ValueType::ValueBoolean, ValueType::ValueBoolean ), ValueType::ValueBoolean },
  { BinaryOperatorValue( "^", ValueType::ValueBoolean, ValueType::ValueBoolean ), ValueType::ValueBoolean },
  { BinaryOperatorValue( "<", ValueType::ValueConstant, ValueType::ValueConstant ), ValueType::ValueBoolean },
  { BinaryOperatorValue( ">", ValueType::ValueConstant, ValueType::ValueConstant ), ValueType::ValueBoolean },
  { BinaryOperatorValue( "<=", ValueType::ValueConstant, ValueType::ValueConstant ), ValueType::ValueBoolean },
  { BinaryOperatorValue( ">=", ValueType::ValueConstant, ValueType::ValueConstant ), ValueType::ValueBoolean },
  { BinaryOperatorValue( "==", ValueType::ValueConstant, ValueType::ValueConstant ), ValueType::ValueBoolean },
  { BinaryOperatorValue( "!=", ValueType::ValueConstant, ValueType::ValueConstant ), ValueType::ValueBoolean },

  /* derived constant binary values */
  { BinaryOperatorValue( "+", ValueType::ValueConstant, ValueType::ValueConstant ), ValueType::ValueConstant },
  { BinaryOperatorValue( "-", ValueType::ValueConstant, ValueType::ValueConstant ), ValueType::ValueConstant },
  { BinaryOperatorValue( "*", ValueType::ValueConstant, ValueType::ValueConstant ), ValueType::ValueConstant },
  { BinaryOperatorValue( "/", ValueType::ValueConstant, ValueType::ValueConstant ), ValueType::ValueConstant },
};

class Node {
  protected:
    std::string text;
    NodeType nodeType;
    ValueType valueType;
    Position position;

  public:
    Node( const std::string _text, const NodeType _type, const Position _position )
      : text( _text ), nodeType( _type ), position( _position ) {
      valueType = ValueType::ValueVoid;
    }

    virtual ~Node() {}

    std::string getText() const {
      return text;
    }

    NodeType getNodeType() const {
      return nodeType;
    }

    virtual ValueType getValueType() const {
      return valueType;
    }

    Position getPosition() const {
      return position;
    }

    void setValueType( const ValueType _valueType ) {
      valueType = _valueType;
    }
};

class BooleanNode : public Node {
  public:
    BooleanNode( const std::string _text, const Position _position )
      : Node( _text, NodeType::NodeBoolean, _position ) {
      setValueType( ValueType::ValueBoolean );
    }
};

class ConstantNode : public Node {
  public:
    ConstantNode( const std::string _text, const Position _position ) 
      : Node( _text, NodeType::NodeConstant, _position ) {
      setValueType( ValueType::ValueConstant );
    }
};

class VariableNode : public Node {
  public:
    VariableNode( const std::string _text, const Position _position )
      : Node( _text, NodeType::NodeVariable, _position ) {}
    
    /* refer to environment to derive type */
    ValueType getValueType() const {
      return getVariableValueType( text );
    }
};

class BindingNode : public Node {
  private:
    Node* bindingExpression;

  public:
    BindingNode( const std::string _text, const Position _position, Node* _bindingExpression )
      : Node( _text, NodeType::NodeBinding, _position ), bindingExpression( _bindingExpression ) {
      addVariable( text, bindingExpression->getValueType() );
    }

    ~BindingNode() {
      if ( bindingExpression ) delete bindingExpression;
    }

    Node* getBindingExpression() const {
      return bindingExpression;
    }
};

class UnaryOperatorNode : public Node {
  private:
    Node* operand;

  public:
    UnaryOperatorNode( const std::string _text, const Position _position, Node* _operand )
      : Node( _text, NodeType::NodeUnaryOperator, _position ), operand( _operand ) {
      setValueType( operand->getValueType() );
    }

    ~UnaryOperatorNode() {
      if ( operand ) delete operand;
    }

    Node* getOperand() const {
      return operand;
    }
};

class BinaryOperatorNode : public Node {
  private:
    Node* lOperand;
    Node* rOperand;

  public:
    BinaryOperatorNode( const std::string _text, const Position _position, Node* _lOperand, Node* _rOperand )
      : Node( _text, NodeType::NodeBinaryOperator, _position ), lOperand( _lOperand ), rOperand( _rOperand ) {
      ValueType leftValueType = lOperand->getValueType();
      ValueType rightValueType = rOperand->getValueType();

      ValueType derivedValueType = mapping(
        DERIVED_BINARY_VALUES,
        BinaryOperatorValue( text, leftValueType, rightValueType ),
        ValueType::ValueUndefined
      );

      if ( derivedValueType == ValueType::ValueUndefined ) {
        log(
          Severity::Error,
          _position,
          ERR_BINARY_VALUES_NOT_SUPPORTED,
          _text,
          _lOperand->getValueType(),
          _rOperand->getValueType()
        );
      }

      setValueType( derivedValueType );
    }

    ~BinaryOperatorNode() {
      if ( lOperand ) delete lOperand;
      if ( rOperand ) delete rOperand;
    }

    Node* getLeftOperand() const {
      return lOperand;
    }

    Node* getRightOperand() const {
      return rOperand;
    }
};

class MultiStatementNode : public Node {
  private:
    std::vector<Node*> statements;

  public:
    MultiStatementNode( const Position _position, std::vector<Node*> _statements )
      : Node( "", NodeType::NodeMultiStatement, _position ), statements( _statements ) {}

    ~MultiStatementNode() {
      for ( Node* statement : statements ) {
        delete statement;
      }
    }

    std::vector<Node*> getStatements() const {
      return statements;
    }
};

class ConditionalNode : public Node {
  private:
    Node* conditional;
    Node* upperBody;
    Node* lowerBody;

  public:
    ConditionalNode( const Position _position, Node* _conditional, Node* _upperBody, Node* _lowerBody )
      : Node( "if-else", NodeType::NodeConditional, _position ),
        conditional( _conditional ),
        upperBody( _upperBody ),
        lowerBody( _lowerBody ) {}
    
    ~ConditionalNode() {
      if ( conditional ) delete conditional;
      if ( upperBody ) delete upperBody;
      if ( lowerBody ) delete lowerBody;
    }

    Node* getConditional() const {
      return conditional;
    }

    Node* getUpperBody() const {
      return upperBody;
    }

    Node* getLowerBody() const {
      return lowerBody;
    }
};

class FunctionCallNode : public Node {
  private:
    std::string name;
    std::vector<Node*> arguments;

  public:
    FunctionCallNode( const Position _position, const std::string _name, std::vector<Node*> _arguments )
      : Node( _name, NodeType::NodeFunctionCall, _position ), name( _name ), arguments( _arguments ) {}
    
    ~FunctionCallNode() {
      for ( Node* argument : arguments ) {
        delete argument;
      }
    }
    
    std::string getName() const {
      return name;
    }

    std::vector<Node*> getArguments() const {
      return arguments;
    }

    int getArgumentCount() const {
      return arguments.size();
    }
};

#endif
