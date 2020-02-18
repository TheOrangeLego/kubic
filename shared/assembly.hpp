#ifndef _ASSEMBLY_HPP
#define _ASSEMBLY_HPP

#include <string>
#include <map>
#include <boost/format.hpp>

enum Register {
  /* general purpose */
  RAX, RBX, RCX, RDX,

  /* index registers */
  RSI, RDI,

  /* stack frame */
  RSP, RBP
};

static std::map<Register, std::string> registerMap = {
  {RAX, "rax"}, {RBX, "rbx"}, {RCX, "rcx"}, {RDX, "rdx"},

  {RSI, "rsi"}, {RDI, "rdi"},

  {RSP, "rsp"}, {RBP, "rbp"},
};

std::string reg( const Register _register ) {
  return registerMap[_register];
}

std::string regOffset( const Register _register, const unsigned int _offset ) {
  return ( boost::format( "[%1% - %2%]" ) % reg( _register ) % std::to_string( 8 * _offset ) ).str();
}

#endif
