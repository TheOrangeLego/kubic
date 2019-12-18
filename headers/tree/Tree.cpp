#include "Tree.hpp"

std::string ConstNode::print() const {
  *representation << "  mov eax, " << constantNode.getToken() << std::endl;

  return representation->str();
}

std::string BinaryOperator::print() const {
  *representation << rightNode->print();
  *representation << "  mov ebx, eax" << std::endl;
  *representation << leftNode->print();
  
  if ( !op.compare( "+" ) ) {
    *representation << "  add eax, ebx" << std::endl;
  } else if ( !op.compare( "-" ) ) {
    *representation << "  add eax, ebx" << std::endl;
  } else {

  }

  return representation->str();
}
