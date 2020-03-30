#ifndef _RULES_HPP
#define _RULES_HPP

#include <map>
#include <set>
#include <string>

#include "../shared/token.hpp"

const std::string WHITESPACE_CHARS = " \t";

const std::string NEWLINE_CHARS = "\n\r";

const std::string OPERATOR_CHARS = "+-*/";

const std::string GROUP_CHARS = "()";

const std::set<std::string> KEYWORDS {
};

const std::set<std::string> UNARY_OPERATORS {
  "!"
};

const std::set<std::string> BINARY_OPERATORS {
  "+", "-", "*", "/",
  ">", "<", ">=", "<=",
  "=", "=="
};

const std::set<std::string> DATA_TYPES {
  "Void",
  "Integer",
  "String"
};

std::map<std::string, unsigned int> OPERATOR_PRIORITIES = {
  {"(", 0}, {")", 0},
  {"+", 1}, {"-", 1},
  {"*", 2}, {"/", 2}
};

std::map<std::string, unsigned int> DATA_TYPE_MAPS = {
  {"Void",    DataType::VoidData},
  {"Integer", DataType::IntegerData},
  {"String",  DataType::StringData},
};

bool hasHigherPriority( const Token _tokenA, const Token _tokenB ) {
  return OPERATOR_PRIORITIES[ _tokenA.getText()] > OPERATOR_PRIORITIES[_tokenB.getText()];
}

#endif
