#ifndef _TYPES_HPP
#define _TYPES_HPP

#include <map>
#include <string>

enum TokenType {
  TokenUndefined,
  TokenComma,
  TokenNewline,
  TokenBoolean,
  TokenConstant,
  TokenVariable,
  TokenKeyword,
  TokenOperator,
  TokenArithmeticGrouper,
  TokenStatementGrouper,
};

enum NodeType {
  NodeUndefined,
  NodeBoolean,
  NodeConstant,
  NodeVariable,
  NodeBinding,
  NodeUnaryOperator,
  NodeBinaryOperator,
  NodeMultiStatement,
  NodeConditional,
  NodeFunctionCall,
};

enum ValueType {
  ValueUndefined,
  ValueVoid,
  ValueBoolean,
  ValueConstant,
};

std::map<std::string, ValueType> VALUE_TYPE_NAME = {
  { "boolean", ValueType::ValueBoolean },
  { "number", ValueType::ValueConstant },
};

ValueType translateToValueType( const std::string _type ) {
  if ( VALUE_TYPE_NAME.find( _type ) == VALUE_TYPE_NAME.end() ) {
    return ValueType::ValueUndefined;
  }

  return VALUE_TYPE_NAME.at( _type );
}

std::string translateFromValueType( const ValueType _type ) {
  switch ( _type ) {
    case ValueVoid:
      return "void";
      break;
    case ValueBoolean:
      return "boolean";
      break;
    case ValueConstant:
      return "number";
      break;
    default:
      return "undefined";
      break;
  }
}

#endif
