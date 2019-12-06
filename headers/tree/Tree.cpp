#include "Tree.hpp"

Token TreeNode::getToken() const {
  return this->token;
}

std::string ConstNode::print() const {
  return this->token.getToken();
}
