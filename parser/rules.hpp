#ifndef _RULES_HPP
#define _RULES_HPP

#include <string>
#include <set>
#include <map>

const std::string WHITESPACE_CHARS = " \t";

const std::string NEWLINE_CHARS = "\n\r";

const std::string OPERATOR_CHARS = "+-*/";

const std::string GROUP_CHARS = "()";

const std::set<std::string> KEYWORDS {
  "let"
};

const std::set<std::string> UNARY_OPERATORS {};

const std::set<std::string> BINARY_OPERATORS {
  "+", "-", "*", "/"
};

std::map<std::string, unsigned int> OPERATOR_PRIORITIES = {
  {"(", 0}, {")", 0},
  {"+", 1}, {"-", 1},
  {"*", 2}, {"/", 2}
};

bool hasHigherPriority( const Token _tokenA, const Token _tokenB ) {
  return OPERATOR_PRIORITIES[ _tokenA.getToken()] > OPERATOR_PRIORITIES[_tokenB.getToken()];
}

#endif
