#include "Tree.hpp"

#include <boost/format.hpp>

#include "registers.hpp"
#include "../helpers.hpp"

std::string ConstantNode::compile( const unsigned int, EnvironmentMap& ) const {
  *representation << boost::format( "  mov qword rax, %1%\n" ) % constant.getToken();

  return representation->str();
}

std::string VariableNode::compile( const unsigned int, EnvironmentMap& _bindings ) const {
   unsigned int offset = 0;

   for ( std::pair<std::string, unsigned int> pair : _bindings ) {
     if ( equals( pair.first, variable.getToken() ) ) {
       offset = pair.second;
       break;
     }
   }

  *representation << boost::format( "  mov rax, %1%\n" ) % regOffset( Register::RSI, offset );

  return representation->str();
}

std::string UnaryOperator::compile( const unsigned int _stackOffset, EnvironmentMap& _bindings ) const {
  *representation << node->compile( _stackOffset + 1, _bindings );

  return representation->str();
}

std::string BinaryOperator::compile( const unsigned int _stackOffset, EnvironmentMap& _bindings ) const {
  *representation << rightNode->compile( _stackOffset, _bindings );
  *representation << boost::format( "  mov %1%, rax\n" ) % regOffset( Register::RSI, _stackOffset );
  *representation << leftNode->compile( _stackOffset + 1, _bindings );
  
  if ( equals( op.getToken(), "+" ) ) {
    *representation << boost::format( "  add rax, %1%\n" ) % regOffset( Register::RSI, _stackOffset );
  } else if ( equals( op.getToken(), "-" ) ) {
    *representation << boost::format( "  sub rax, %1%\n" ) % regOffset( Register::RSI, _stackOffset );
  } else if ( equals( op.getToken(), "*" ) ) {
    *representation << boost::format( "  imul rax, %1%\n" ) % regOffset( Register::RSI, _stackOffset );
  } else if ( equals( op.getToken(), "/" ) ) {
    *representation << boost::format( "  idiv rax, %1%\n" ) % regOffset( Register::RSI, _stackOffset );
  }

  return representation->str();
}

std::string BindingNode::compile( const unsigned int _stackOffset, EnvironmentMap& _bindings ) const {
  EnvironmentMap newBindings( _bindings );

  *representation << bindingNode->compile( _stackOffset, newBindings );
  *representation << boost::format( "  mov %1%, rax\n" ) % regOffset( Register::RSI, _stackOffset );
  newBindings.push_back( std::pair<std::string, unsigned int>( variable.getToken(), _stackOffset ) );
  *representation << bodyNode->compile( _stackOffset + 1, newBindings );

  return representation->str();
}
