#ifndef _RULES_HPP
#define _RULES_HPP

#include <map>
#include <set>
#include <string>

#include "shared/token.hpp"
#include "shared/types.hpp"

const std::string WHITESPACE_CHARS = " \t";

const std::string NEWLINE_CHARS = "\n\r";

const std::string OPERATOR_CHARS = "+-*/";

const std::string GROUP_CHARS = "()";

const std::set<std::string> CONSTANT_KEYWRDS {
  "false",
  "true",
};

const std::set<std::string> KEYWORDS {
  /* data type keywords */
  "Boolean", "Integer", "String",

  /* keywords */
  "let",
};

const std::set<std::string> UNARY_OPERATORS {
  "!",
};

const std::set<std::string> BINARY_OPERATORS {
  "+", "-", "*", "/",
  ">", "<", ">=", "<=",
  "=", "==",
};

typedef std::tuple<NodeType, std::string, NodeType> BinaryTypeInference;

const std::map<BinaryTypeInference, NodeType> BINARY_OPERATOR_INFERED_TYPES = {
  { BinaryTypeInference( NodeType::NodeInteger, "+", NodeType::NodeInteger ), NodeType::NodeInteger },
  { BinaryTypeInference( NodeType::NodeInteger, "-", NodeType::NodeInteger ), NodeType::NodeInteger },
  { BinaryTypeInference( NodeType::NodeInteger, "*", NodeType::NodeInteger ), NodeType::NodeInteger },
  { BinaryTypeInference( NodeType::NodeInteger, "/", NodeType::NodeInteger ), NodeType::NodeInteger },
};

std::map<std::string, unsigned int> OPERATOR_PRIORITIES = {
  { "(", 0 }, { ")", 0 },
  { "+", 1 }, { "-", 1 },
  { "*", 2 }, { "/", 2 },
};

bool hasHigherPriority( const Token _tokenA, const Token _tokenB ) {
  return OPERATOR_PRIORITIES[ _tokenA.getText()] >= OPERATOR_PRIORITIES[_tokenB.getText()];
};

#endif
