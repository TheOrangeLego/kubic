#ifndef _TYPES_HPP
#define _TYPES_HPP

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
  NodeBinding
};

const std::map<NodeType, std::string> NODE_TYPE_STRING = {
  { NodeUndefined, "undefined" },
  { NodeBoolean, "Boolean" },
  { NodeInteger, "Integer" },
  { NodeString, "String" },
  { NodeBinding, "undefined" }
};

#endif
