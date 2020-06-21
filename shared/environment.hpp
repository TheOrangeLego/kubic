#ifndef _ENVIRONMENT_HPP
#define _ENVIRONMENT_HPP

#include <stack>
#include <vector>

#include "shared/helpers.hpp"
#include "shared/token.hpp"
#include "shared/types.hpp"

typedef std::pair<Token, NodeType> Binding;

class Environment {
  private:
    unsigned int currentVariableCount;
    unsigned int lifetimeVariableCount;
    std::stack<unsigned int> frameVariableCounts;
    std::vector<Binding> bindings;

  public:
    Environment() {
      currentVariableCount = 0;
      lifetimeVariableCount = 0;
    }

    unsigned int currentOffset() const {
      return lifetimeVariableCount;
    }

    bool bindingExists( const Token _token ) const {
      for ( Binding binding : bindings ) {
        if ( binding.first == _token ) {
          return true;
        }
      }

      return false;
    }

    unsigned int getOffset( const Token _token ) const {
      unsigned index = 0;
      for ( Binding binding : bindings ) {
        if ( binding.first == _token ) {
          return index;
        }

        index++;
      }

      return 0;
    }

    NodeType getType( const Token _token ) const {
      for ( Binding binding : bindings ) {
        if ( binding.first == _token ) {
          return binding.second;
        }
      }

      return NodeType::NodeUndefined;
    }

    void addFrame() {
      frameVariableCounts.push( currentVariableCount );
      currentVariableCount = 0;
    }

    void removeFrame() {
      for ( unsigned int count = 0; count < currentVariableCount; count++ ) {
        bindings.pop_back();
      }

      lifetimeVariableCount -= currentVariableCount;
      currentVariableCount = frameVariableCounts.top();
      frameVariableCounts.pop();
    }

    void insertBinding( const Token _token, NodeType _type ) {
      bindings.push_back( Binding( _token, _type ) );
      currentVariableCount++;
      lifetimeVariableCount++;
    }
};

#endif
