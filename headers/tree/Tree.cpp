#include "Tree.hpp"

std::string TreeNode::getTokenString() const {
  return this->token.getToken();
}

TokenType TreeNode::getTokenType() const {
  return this->token.getType();
}

TokenPosition TreeNode::getTokenPosition() const {
  return this->token.getPosition();
}

std::string ConstNode::print() const {
  std::stringstream output;
  output << "  mov eax, " << getTokenString() << std::endl;
  return output.str();
}
