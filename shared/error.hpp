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
  ERR_UNEXPECTED_TERMINATION = "unexpected end of statement after [%2%]",
  ERR_EXPECTED_TOKEN = "unexpected token [%1%], was expecting token [%2%]",
  ERR_EXPECTED_TERMINATION = "expected end of statement after token [%1%]";

/* compiler error messages */
const std::string
  ERR_UNDEFINED_VARIABLE = "undefined variable '%1%' referenced",
  ERR_UNARY_OPERATOR_TYPE_MISMATCH = "invalid type used for operator [%1%], given [%2%] type",
  ERR_BINARY_OPERATOR_TYPE_MISMATCH = "invalid types used for operator '%1%', given left [%2%] and right [%3%] types",
  ERR_BINDING_BODY_TYPE_MISMATCH = "cannot bind variable of type [%1%] to body of type [%2%]";

/* forward declaration */
class Node;

class ErrorLogger {
  protected:
    std::stringstream errors;

    void logError( boost::format& _message ) {
      errors << " -- " << boost::str( _message ) << std::endl;
    }

    template<typename... Arguments>
    void logError( boost::format& _message, Token _token, Arguments... _arguments ) {
      logError( _message % _token.getText(), _arguments... );
    }

    template<typename... Arguments>
    void logError( boost::format& _message, Node* _node, Arguments... _arguments ) {
      logError( _message % NODE_TYPE_STRING.at( _node->getNodeType() ), _arguments... );
    }

  public:
    ErrorLogger() {
      /* literally does nothing */
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

    template<typename... Arguments>
    void logError( std::string _errorMessage, Arguments... _arguments ) {
      boost::format baseMessage( _errorMessage );
      logError( baseMessage, _arguments... );
    }
};

#endif
