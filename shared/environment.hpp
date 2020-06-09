#ifndef _ENVIRONMENT_HPP
#define _ENVIRONMENT_HPP

#include <map>
#include <stack>
#include <set>

#include "shared/helpers.hpp"
#include "shared/token.hpp"
#include "shared/types.hpp"

typedef std::set<Token> FrameVariables;
typedef std::map<Token, unsigned int> FrameOffsets;
typedef std::map<Token, NodeType> FrameTypes;

class Environment {
  private:
    std::stack<FrameVariables> variables;
    std::stack<FrameOffsets> offsets;
    std::stack<FrameTypes> types;

  public:
    Environment() {
      addFrame();
    }

    unsigned int currentOffset() const {
      return variables.top().size();
    }

    bool bindingExists( const Token _token ) const {
      return contains( variables.top(), _token );
    }

    unsigned int getOffset( const Token _token ) const {
      return offsets.top().at( _token );
    }

    NodeType getType( const Token _token ) const {
      return types.top().at( _token );
    }

    void addFrame() {
      variables.push( FrameVariables() );
      offsets.push( FrameOffsets() );
      types.push( FrameTypes() );
    }

    void removeFrame() {
      variables.pop();
      offsets.pop();
      types.pop();
    }

    void insertBinding( const Token _token, NodeType _type ) {
      offsets.top().insert( std::pair<Token, unsigned int>( _token, currentOffset() ) );
      types.top().insert( std::pair<Token, NodeType>( _token, _type ) );
      variables.top().insert( _token );
    }
};

#endif
