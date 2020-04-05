#ifndef _TYPES_HPP
#define _TYPES_HPP

enum TokenType {
  UndefinedTokenType,
  ConstantTokenType,
  VariableTokenType,
  OperatorTokenType,
  GroupTokenType,
  KeywordTokenType
};

enum NodeType {
  ConstantNodeType,
  VariableNodeType,
  UnaryOperatorNodeType,
  BinaryOperatorNodeType,
  BindingNodeType
};

enum DataType {
  VoidData,
  IntegerData,
  StringData
};

#endif
