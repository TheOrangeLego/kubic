#ifndef _ASSEMBLY_HPP
#define _ASSEMBLY_HPP

#include <boost/format.hpp>
#include <iostream>
#include <map>
#include <string>

#include "parser/node.hpp"

const std::string ASM_FALSE = "0x7FFFFFFFFFFFFFFF";

const std::string ASM_TRUE = "0xFFFFFFFFFFFFFFFF";

enum Register {
  RAX, RBX, RCX, RDX,

  RSI, RDI,

  RSP, RBP,
};

const std::map<Register, std::string> REGISTER_MAP = {
  { Register::RAX, "rax" }, { Register::RBX, "rbx" }, { Register::RCX, "rcx" }, { Register::RDX, "rdx" },

  { Register::RSI, "rsi" }, { Register::RDI, "rdi" },

  { Register::RSP, "rsp" }, { Register::RBP, "rbp" },
};

std::string reg( const Register _register ) {
  return REGISTER_MAP.at( _register );
}

std::string regOffset( const Register _register, const signed int _offset ) {
  if ( _offset < 0 ) {
    return ( boost::format( "[%1% + %2%]" ) % reg( _register ) % std::to_string( _offset * -8 ) ).str();
  } else {
    return ( boost::format( "[%1% - %2%]" ) % reg( _register ) % std::to_string( _offset * 8 ) ).str();
  }
}

std::string insn( const std::string _operator, const Register _register, const std::string _src ) {
  return ( boost::format( "  %1% qword %2%, %3%\n" ) % _operator % reg( _register ) % _src ).str(); 
}

std::string insn( const std::string _operator, const std::string _dst, const Register _register ) {
  return ( boost::format( "  %1% %2%, %3%\n" ) % _operator % _dst % reg( _register ) ).str(); 
}

std::string insn( const std::string _operator, const Register _registerA, const Register _registerB ) {
  return ( boost::format( "  %1% %2%, %3%\n" ) % _operator % reg( _registerA ) % reg( _registerB ) ).str(); 
}

std::string pushInsn( const Register _register ) {
  return ( boost::format( "  push %1%\n" ) % reg( _register ) ).str();
}

std::string pushInsn( const std::string _value ) {
  return ( boost::format( "  push %1%\n" ) % _value ).str();
}

std::string popInsn( const Register _register ) {
  return ( boost::format( "  pop %1%\n" ) % reg( _register ) ).str(); 
}

std::string jumpNotTrue( const std::string _labelPrefix, const unsigned int _labelCounter ) {
  return (
    boost::format( "  cmp %1%, %2%\n  jne %3%_%4%\n" ) % reg( Register::RAX ) % ASM_TRUE % _labelPrefix % _labelCounter
  ).str();
}

std::string jump( const std::string _labelPrefix, const unsigned int _labelCounter ) {
  return ( boost::format( "  jmp %1%_%2%\n" ) % _labelPrefix % _labelCounter ).str();
}

std::string label( const std::string _labelPrefix, const unsigned int _labelCounter ) {
  return ( boost::format( "%1%_%2%:\n" ) % _labelPrefix % _labelCounter ).str();
}

std::string call( const std::string _function ) {
  return ( boost::format( "  call %1%\n" ) % _function ).str();
}

std::string formatValue( const Node* _node ) {
  int convertedValue;
  std::string value = _node->getText();
  std::string base = value.substr( 0, 2 );
  ValueType valueType = _node->getValueType();

  switch( valueType ) {
    case ValueType::ValueBoolean:
      if ( value == "false" ) {
        return ASM_FALSE;
      } else {
        return ASM_TRUE;
      }
      break;
    case ValueType::ValueConstant:
      if ( base == "0b" ) {
        convertedValue = std::stoi( value.substr( 2 ), nullptr, 2 );
      } else if ( base == "0o" ) {
        convertedValue = std::stoi( value.substr( 2 ), nullptr, 8 );
      } else if ( base == "0d" ) {
        convertedValue = std::stoi( value.substr( 2 ), nullptr, 10 );
      } else if ( base == "0x" ) {
        convertedValue = std::stoi( value.substr( 2 ), nullptr, 16 );
      } else {
        convertedValue = std::stoi( value );
      }

      convertedValue = convertedValue << 1;

      return std::to_string( convertedValue );
      break;
    default:
      return "";
      break;
  }
}

#endif
