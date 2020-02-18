#ifndef _MESSAGES_HPP
#define _MESSAGES_HPP

#include <string>

#include "token/Token.hpp"

typedef std::pair<Token, std::string> ParserError;

/* rework format of error and warning messages */
const std::string ERR_UNEXPECTED_TOKEN = "unexpected token";

const std::string ERR_MISSING_LEFT_OPERAND = "missing left operand";
const std::string ERR_MISSING_RIGHT_OPERAND = "missing right operand";
const std::string ERR_UNEXPECTED_EXPR_END = "unexpected end of expression";

const std::string ERR_MISSING_VARIABLE_BINDING = "missing variable to bind";
const std::string ERR_MISSING_ASSIGNMENT_BINDING = "missing binding assignment operator \"=\"";
const std::string ERR_MISSING_ENDING_BINDING = "missing variable assignment terminator \"::\"";
const std::string ERR_EXPECTED_ASSIGNMENT_BINDING = "expected binding assignment operator \"=\"";
const std::string ERR_EXPECTED_ENDING_BINDING = "expected variable assignment terminator \"::\"";

#endif
