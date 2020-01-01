#include "Tree.hpp"

#include <boost/format.hpp>

#include "registers.hpp"
#include "../helpers.hpp"

std::string ConstNode::compile( const unsigned int _stackOffset ) const {
  *representation << boost::format( "  mov qword %1%, %2%\n" ) % regOffset( Register::RSI, _stackOffset ) % constantNode.getToken();

  return representation->str();
}

std::string UnaryOperator::compile( const unsigned int _stackOffset ) const {
  *representation << node->compile( _stackOffset + 1 );

  return representation->str();
}

std::string BinaryOperator::compile( const unsigned int _stackOffset ) const {
  *representation << leftNode->compile( _stackOffset );
  *representation << rightNode->compile( _stackOffset + 1 );
  *representation << boost::format( "  mov rax, %1%\n" ) % regOffset( Register::RSI, _stackOffset );
  
  if ( equals( op.getToken(), "+" ) ) {
    *representation << boost::format( "  add rax, %1%\n" ) % regOffset( Register::RSI, _stackOffset + 1 );
  } else if ( equals( op.getToken(), "-" ) ) {
    *representation << boost::format( "  sub rax, %1%\n" ) % regOffset( Register::RSI, _stackOffset + 1 );
  } else if ( equals( op.getToken(), "*" ) ) {
    *representation << boost::format( "  imul rax, %1%\n" ) % regOffset( Register::RSI, _stackOffset + 1 );
  } else if ( equals( op.getToken(), "/" ) ) {
    *representation << boost::format( "  idiv rax, %1%\n" ) % regOffset( Register::RSI, _stackOffset + 1 );
  }

  *representation << boost::format( "  mov %1%, rax\n" ) % regOffset( Register::RSI, _stackOffset );

  return representation->str();
}

std::string BindingNode::compile( const unsigned int _stackOffset ) const {
  *representation << bindingNode->compile( _stackOffset );
  *representation << bodyNode->compile( _stackOffset + 1 );

  return representation->str();
}
