#ifndef _TYPES_HPP
#define _TYPES_HPP

#include <string>

#include "shared/helpers.hpp"

enum TokenType {
  TokenUndefined,
  TokenNewline,
  TokenConstant,
  TokenVariable,
  TokenGroup,
  TokenKeyword,
  TokenOperator,
};

enum NodeType {
  NodeUndefined,
  NodeBoolean,
  NodeInteger,
  NodeString,
  NodeVariable,
};

std::string typeToString( const NodeType _nodeType ) {
  if ( _nodeType == NodeType::NodeBoolean ) {
    return "Boolean";
  } else if ( _nodeType == NodeType::NodeInteger ) {
    return "Integer";
  } else if ( _nodeType == NodeType::NodeString ) {
    return "String";
  } else {
    return "Undefined";
  }
}

NodeType stringToType( const std::string _string ) {
  if ( _string == "Boolean" ) {
    return NodeType::NodeBoolean;
  } else if ( _string == "Integer" ) {
    return NodeType::NodeInteger;
  } else if ( _string == "String" ) {
    return NodeType::NodeString;
  } else {
    return NodeType::NodeUndefined;
  }
}

#endif
