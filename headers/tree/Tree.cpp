#include "Tree.hpp"

std::string ConstNode::print() const {
  std::stringstream output;

  output << "  mov eax, " << constant.getToken() << std::endl;

  return output.str();
}

std::string BinaryOperator::print() const {
  std::stringstream output;

  output << rightNode->print();
  output << "  mov ebx, eax" << std::endl;
  output << leftNode->print();
  
  if ( !op.compare( "+" ) ) {
    output << "  add eax, ebx" << std::endl;
  } else if ( !op.compare( "-" ) ) {
    output << "  add eax, ebx" << std::endl;
  } else {

  }

  return output.str();
}
