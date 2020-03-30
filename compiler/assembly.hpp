#ifndef _ASSEMBLY_HPP
#define _ASSEMBLY_HPP

#include <map>
#include <string>

#include "../shared/node.hpp"
#include "../shared/helpers.hpp"

enum Register {
  /* general purpose */
  RAX, RBX, RCX, RDX,

  /* index registers */
  RSI, RDI,

  /* stack frame */
  RSP, RBP
};

static std::map<Register, std::string> REGISTER_MAP = {
  {RAX, "rax"}, {RBX, "rbx"}, {RCX, "rcx"}, {RDX, "rdx"},

  {RSI, "rsi"}, {RDI, "rdi"},

  {RSP, "rsp"}, {RBP, "rbp"},
};

std::string reg( const Register _register ) {
  return REGISTER_MAP[_register];
}

std::string regOffset( const Register _register, const unsigned int _offset ) {
  return ( boost::format( "[%1% - %2%]" ) % reg( _register ) % std::to_string( 8 * _offset ) ).str();
}

std::string ConstantNode::compile( const unsigned int _stackOffset, EnvironmentBindings& _bindings ) const {
  return printSingle( "  mov qword rax, %1%\n", constant.getText() );
}

std::string VariableNode::compile( const unsigned int _stackPffset, EnvironmentBindings& _bindings ) const {
  for ( unsigned int index = _bindings.size() - 1; index >= 0; index-- ) {
    if ( equals( _bindings[index].first, variable ) ) {
      return printSingle( "  mov rax, %1%\n", regOffset( RSI, _bindings[index].second ) );
    }
  }
}

std::string UnaryOperatorNode::compile( const unsigned int _stackOffset, EnvironmentBindings& _bindings ) const {
  *representation << operand->compile( _stackOffset, _bindings );

  return representation->str();
}

std::string BinaryOperatorNode::compile( const unsigned int _stackOffset, EnvironmentBindings& _bindings ) const {
  *representation << rOperand->compile( _stackOffset, _bindings );
  formatSingle( representation, "  mov %1%, rax\n", regOffset( RSI, _stackOffset ) );
  *representation << lOperand->compile( _stackOffset + 1, _bindings );

  if ( equals( bOperator.getText(), "+" ) ) {
    formatSingle( representation, "  add rax, %1%\n", regOffset( RSI, _stackOffset ) );
  } else if ( equals( bOperator.getText(), "-" ) ) {
    formatSingle( representation, "  sub rax, %1%\n", regOffset( RSI, _stackOffset ) );
  } else if ( equals( bOperator.getText(), "*" ) ) {
    formatSingle( representation, "  imul rax, %1%\n", regOffset( RSI, _stackOffset ) );
  } else if ( equals( bOperator.getText(), "/" ) ) {
    formatSingle( representation, "  idiv rax, %1%\n", regOffset( RSI, _stackOffset ) );
  }

  return representation->str();
}

std::string BindingNode::compile( const unsigned int _stackOffset, EnvironmentBindings& _bindings ) const {
  /* TODO -- implement binding with types */
  return representation->str();
}

#endif
