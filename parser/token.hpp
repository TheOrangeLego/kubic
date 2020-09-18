#ifndef _TOKEN_HPP
#define _TOKEN_HPP

#include <set>
#include <string>

#include "shared/position.hpp"
#include "shared/types.hpp"

const std::set<TokenType> ARITHMETIC_TYPES = {
  TokenType::TokenBoolean,
  TokenType::TokenConstant,
  TokenType::TokenVariable,
  TokenType::TokenOperator,
  TokenType::TokenArithmeticGrouper,
};

class Token {
  private:
    std::string text;
    TokenType type;
    Position position;

  public:
    Token( const std::string _text, const TokenType _type, const Position _position )
      : text( _text ), type( _type ), position( _position ) {}

    Token(
      const std::string _text,
      const TokenType _type,
      const unsigned int _line,
      const unsigned int _column,
      const std::string _filename
    ) : text( _text ), type( _type ), position( Position( _line, _column, _filename ) ) {}

    std::string getText() const {
      return text;
    }

    TokenType getType() const {
      return type;
    }

    Position getPosition() const {
      return position;
    }
};

#endif
