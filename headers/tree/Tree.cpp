#include "Tree.hpp"

#include "../helpers.hpp"

std::string ConstNode::print() const {
  *representation << "  mov rax, " << constantNode.getToken() << std::endl;

  return representation->str();
}

std::string UnaryOperator::print() const {
  *representation << node->print();

  return representation->str();
}

std::string BinaryOperator::print() const {
  *representation << rightNode->print();
  *representation << "  mov rbx, rax" << std::endl;
  *representation << leftNode->print();
  
  if ( equals( op.getToken(), "+" ) ) {
    *representation << "  add rax, rbx" << std::endl;
  } else if ( equals( op.getToken(), "-" ) ) {
    *representation << "  sub rax, rbx" << std::endl;
  } else if ( equals( op.getToken(), "*" ) ) {
    *representation << "  imul rax, rbx" << std::endl;
  } else if ( equals( op.getToken(), "/" ) ) {
    *representation << "  idiv rax" << std::endl;
  }

  return representation->str();
}
