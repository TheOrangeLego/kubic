#ifndef _ERROR_HPP
#define _ERROR_HPP

#include <boost/format.hpp>
#include <map>
#include <queue>
#include <sstream>

#include "shared/token.hpp"

typedef std::pair<unsigned int, Token> Error;

/* setup and environment errors */
const unsigned int
  INFRA_ERROR_BOUND = 200;

/* parser errors */
const unsigned int
  ERR_UNEXPECTED_TOKEN = 201,
  ERR_UNEXPECTED_TERMINATION = 202,
  ERR_EXPECTED_TOKEN = 203,
  ERR_EXPECTED_TERMINATION = 204,

  PARSER_ERROR_BOUND = 400;

/* compiler errors */
const unsigned int
  COMPILER_ERROR_BOUND = 600;

const std::map<unsigned int, std::string> ERROR_MESSAGES = {
  { ERR_UNEXPECTED_TOKEN, "unexpected token %s" },
  { ERR_UNEXPECTED_TERMINATION, "unexpected termination of statement for %s" },
  { ERR_EXPECTED_TOKEN, "expected token %s" },
  { ERR_EXPECTED_TERMINATION, "expected termination of statement for %s" }
};

class ErrorLogger {
  public:
    ErrorLogger() {
      /* literally does nothing */
    }

    bool empty() const {
      return errors.empty();
    }

    std::stringstream streamErrors() {
      std::stringstream stream;

      while ( !errors.empty() ) {
        stream << translateError( errors.front() ) << std::endl;
        errors.pop();
      }

      return stream;
    }

    void logError( const unsigned int _code, const Token _token ) {
      errors.push( Error( _code, _token ) );
    }

  protected:
    std::queue<Error> errors;

    std::string translateError( const Error _error ) {
      std::string errorMessage;

      if ( errors.empty() ) {
        return errorMessage;
      } else if ( _error.first % 200 < 100 ) {
        errorMessage = ( boost::format( ERROR_MESSAGES.at( _error.first ) ) % _error.second.getText() ).str();
      } else {
        errorMessage = ( boost::format( ERROR_MESSAGES.at( _error.first ) ) % _error.second.getText() ).str();
      }

      errorMessage = ( boost::format( "  %s :: " ) % _error.second.getPosition() ).str() + errorMessage;

      return errorMessage;
    }
};

#endif
