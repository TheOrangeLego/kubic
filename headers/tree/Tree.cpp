#include "Tree.hpp"

std::string ConstNode::print() const {
  *representation << "  mov rax, " << constantNode.getToken() << std::endl;

  return representation->str();
}

std::string BinaryOperator::print() const {
  *representation << rightNode->print();
  *representation << "  mov rbx, rax" << std::endl;
  *representation << leftNode->print();
  
  if ( !op.getToken().compare( "+" ) ) {
    *representation << "  add rax, rbx" << std::endl;
  } else if ( !op.getToken().compare( "-" ) ) {
    *representation << "  sub rbx, rax" << std::endl;
    *representation << "  mov rax, rbx" << std::endl;
  } else {

  }

  return representation->str();
}
