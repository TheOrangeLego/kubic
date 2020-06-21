#ifndef _ERROR_HPP
#define _ERROR_HPP

#include <boost/format.hpp>
#include <map>
#include <queue>
#include <sstream>
#include <stdarg.h>

#include "shared/token.hpp"
#include "shared/types.hpp"

/* parser error messages */
const std::string
  ERR_UNEXPECTED_TOKEN = "unexpected token [%1%]",
  ERR_UNEXPECTED_TERMINATION = "unexpected end of statement after [%1%]",
  ERR_EXPECTED_TOKEN = "unexpected token [%1%], was expecting token [%2%]",
  ERR_EXPECTED_TERMINATION = "expected end of statement after token [%1%]";

/* compiler error messages */
const std::string
  ERR_UNDEFINED_VARIABLE = "undefined variable [%1%] referenced",
  ERR_UNARY_OPERATOR_TYPE_MISMATCH = "invalid type used for operator [%1%], given [%2%] type",
  ERR_BINARY_OPERATOR_TYPE_MISMATCH = "invalid types used for operator [%1%], given left [%2%] and right [%3%] types",
  ERR_BINDING_ALREADY_EXISTS = "cannot rebind variable [%1%]",
  ERR_BINDING_BODY_TYPE_INVALID = "could not bind variable to body expression with invalid type",
  ERR_BINDING_BODY_TYPE_MISMATCH = "cannot bind variable of type [%1%] to body of type [%2%]";

/* forward declaration */
class Node;

class ErrorLogger {
  protected:
    unsigned int errorCount;

    std::stringstream errors;

    std::string escapeString( const std::string _string ) {
      if ( _string == "\n" ) {
        return "{newline}";
      } else {
        return _string;
      }
    }

    void logError( boost::format& _message ) {
      errors << " -- " << boost::str( _message ) << std::endl;

      errorCount++;
    }

    template<typename... Arguments>
    void logError( boost::format& _message, std::string _string, Arguments... _arguments ) {
      logError( _message % escapeString( _string ), _arguments... );
    }

    template<typename... Arguments>
    void logError( boost::format& _message, Token _token, Arguments... _arguments ) {
      logError( _message % escapeString( _token.getText() ), _arguments... );
    }

  public:
    ErrorLogger() {
      errorCount = 0;
    }

    bool empty() {
      int size = 0;

      errors.seekg( 0, std::ios::end );
      size = errors.tellg();
      errors.seekg( 0, std::ios::beg );

      return size == 0;
    }

    std::string getErrors() {
      return errors.str();
    }

    unsigned int getErrorCount() const {
      return errorCount;
    }

    template<typename... Arguments>
    void logError( std::string _errorMessage, Arguments... _arguments ) {
      boost::format baseMessage( _errorMessage );
      logError( baseMessage, _arguments... );
    }

    /* fun times with circular dependencies going on here, logError needs Node */
    /* but Node needs ErrorLogger, so we make it public and define it after Node */
    template<typename... Arguments>
    void logError( boost::format& _message, Node* _node, Arguments... _arguments );
};

#endif
