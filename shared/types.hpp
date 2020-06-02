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
  NodeBoolean,
  NodeInteger,
  NodeString,
  NodeVariable,
  NodeBinding,
  NodeUnaryOperator,
  NodeBinaryOperator,
};

#endif
