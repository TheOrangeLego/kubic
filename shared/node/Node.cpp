#include "Node.hpp"

#include <boost/format.hpp>

#include "../helpers.hpp"
#include "../assembly.hpp"

std::string ConstantNode::compile( const unsigned int _stackOffset, EnvironmentMap& _bindings ) const {
  return ( boost::format( "  mov qword rax, %1%\n" ) % constant.getToken() ).str();
}

std::string VariableNode::compile( const unsigned int _stackOffset, EnvironmentMap& _bindings ) const {
  for ( unsigned int index = _bindings.size() - 1; index >= 0; index-- ) {
    if ( equals( _bindings[index].first, variable ) ) {
      return ( boost::format( "  mov rax, %1%\n" ) % regOffset( RSI, _bindings[index].second ) ).str();
    }
  }

  /* TODO -- variable not found message */ 
}

std::string UnaryOperatorNode::compile( const unsigned int _stackOffset, EnvironmentMap& _bindings ) const {
  *representation << operand->compile( _stackOffset, _bindings );

  /* TODO -- map each possible unary operator to an appropriate assembly instruction */

  return representation->str();
}

std::string BinaryOperatorNode::compile( const unsigned int _stackOffset, EnvironmentMap& _bindings ) const {
  *representation << rOperand->compile( _stackOffset, _bindings );
  *representation << boost::format( "  mov %1%, rax\n" ) % regOffset( RSI, _stackOffset );
  *representation << lOperand->compile( _stackOffset + 1, _bindings );

  if ( equals( operate.getToken(), "+" ) ) {
    *representation << boost::format( "  add rax, %1%\n" ) % regOffset( RSI, _stackOffset );
  } else if ( equals( operate.getToken(), "-" ) ) {
    *representation << boost::format( "  sub rax, %1%\n" ) % regOffset( RSI, _stackOffset );
  } else if ( equals( operate.getToken(), "-" ) ) {
    *representation << boost::format( "  imul rax, %1%\n" ) % regOffset( RSI, _stackOffset );
  } else if ( equals( operate.getToken(), "-" ) ) {
    *representation << boost::format( "  idiv rax, %1%\n" ) % regOffset( RSI, _stackOffset );
  }

  return representation->str();
}

std::string BindingNode::compile( const unsigned int _stackOffset, EnvironmentMap& _bindings ) const {
  EnvironmentMap appendedEnvironment( _bindings );

  *representation << binding->compile( _stackOffset, _bindings );
  appendedEnvironment.push_back( EnvironmentPair( variable, _stackOffset ) );
  *representation << body->compile( _stackOffset + 1, appendedEnvironment );
  return representation->str();
}
