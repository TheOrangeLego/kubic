#ifndef _TYPES_HPP
#define _TYPES_HPP

enum TokenType {
  UndefinedTokenType,
  NewlineTokenType,
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
  IntegerData,
  StringData
};

#endif
