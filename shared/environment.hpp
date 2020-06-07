#ifndef _ENVIRONMENT_HPP
#define _ENVIRONMENT_HPP

#include <map>
#include <set>

#include "shared/helpers.hpp"
#include "shared/token.hpp"
#include "shared/types.hpp"

class Environment {
  private:
    unsigned int offset;

    std::set<Token> variables;
    std::map<Token, unsigned int> offsets;
    std::map<Token, NodeType> types;

  public:
    Environment() {
      offset = 0;
    }

    unsigned int currentOffset() const {
      return offset;
    }

    bool bindingExists( const Token _token ) const {
      return contains( variables, _token );
    }

    unsigned int getOffset( const Token _token ) const {
      return offsets.at( _token );
    }

    NodeType getType( const Token _token ) const {
      return types.at( _token );
    }

    void insertBinding( const Token _token, NodeType _type ) {
      variables.insert( _token );
      offsets.insert( std::pair<Token, unsigned int>( _token, offset++ ) );
      types.insert( std::pair<Token, NodeType>( _token, _type ) );
    }
};

#endif
