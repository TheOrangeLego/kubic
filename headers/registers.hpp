#ifndef _REGISTERS_HPP
#define _REGISTERS_HPP

#include <string>
#include <sstream>
#include <map>

enum Register {
  /* general purpose */
  RAX, RBX, RCX, RDX,

  /* register indices */
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
  return "[" + registerMap[_register] + " - " + std::to_string( 8 * _offset ) + "]";
}

#endif