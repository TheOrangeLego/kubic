#ifndef _TYPES_HPP
#define _TYPES_HPP

enum TokenType {
  UndefinedToken,
  ConstantToken,
  VariableToken,
  OperatorToken,
  GroupToken,
  KeywordToken
};

enum NodeType {
  ConstantNode,
  VariableNode,
  UnaryOperatorNode,
  BinaryOperatorNode,
  BindingNode
};

enum DataType {
  VoidData,
  IntegerData,
  StringData
};

#endif
