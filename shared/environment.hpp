#ifndef _ENVIRONMENT_HPP
#define _ENVIRONMENT_HPP

#include <boost/range/adaptor/reversed.hpp>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include "shared/types.hpp"
#include "shared/utils.hpp"

typedef std::tuple<int, ValueType> VariableInfo;

static std::vector<std::map<std::string, VariableInfo>> bindings;

static std::map<std::string, VariableInfo> currentBindings;

static std::map<std::string, int> functions = {
  { "print", 1 },
};

static int baseOffset = 2;

void addVariable( const std::string _variable, const ValueType _valueType ) {
  currentBindings.insert( { _variable, VariableInfo( baseOffset++, _valueType ) } );
}

VariableInfo getVariable( const std::string _variable ) {
  if ( currentBindings.find( _variable ) != currentBindings.end() ) {
    return currentBindings.at( _variable );
  }

  for ( std::map<std::string, VariableInfo> binding : boost::adaptors::reverse( bindings ) ) {
    if ( binding.find( _variable ) != binding.end() ) {
      return binding.at( _variable );
    }
  }

  return VariableInfo( 0, ValueType::ValueVoid );;
}

int getVariableOffset( const std::string _variable ) {
  return std::get<0>( getVariable( _variable ) );
}

ValueType getVariableValueType( const std::string _variable ) {
  return std::get<1>( getVariable( _variable ) );
}

void pushStack( std::vector<std::tuple<std::string, ValueType>> _parameters ) {
  int parameterOffset = -3;
  baseOffset = 2;
  bindings.push_back( currentBindings );
  currentBindings.clear();

  for ( std::tuple<std::string, ValueType> parameter : _parameters ) {
    std::string identifier = std::get<0>( parameter );
    ValueType valueType = std::get<1>( parameter );
    currentBindings.insert( { identifier, VariableInfo( parameterOffset--, valueType ) } );
  }
}

void popStack() {
  currentBindings = bindings.back();
  bindings.pop_back();
  baseOffset = currentBindings.size();
}

void addFunction( const std::string _name, const int _argumentCount ) {
  functions.insert( { _name, _argumentCount } );
}

int getFunction( const std::string _name ) {
  return mapping( functions, _name, -1 );
}

#endif
