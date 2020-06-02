#ifndef _ASSEMBLY_HPP
#define _ASSEMBLY_HPP

#include <map>
#include <string>

#include "shared/node.hpp"
#include "shared/helpers.hpp"

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

#endif