#ifndef _ERRORS_HPP
#define _ERRORS_HPP

#include <boost/format.hpp>
#include <queue>
#include <stdarg.h>
#include <string>

#include "shared/position.hpp"
#include "shared/types.hpp"

const std::string
  WARN_ = "";

const std::string
  ERR_INVALID_TOKEN = "encountered invalid token %1%",
  ERR_BINARY_VALUES_NOT_SUPPORTED = "operator %1% does not support operation on left %2% and right %3% values",
  ERR_BINDING_TYPE_MISMATCH = "binding expression type %1% does not match expected type %2%";

enum Severity {
  Error,
  Warning,
};

class LogEntry {
  private:
    std::string message;
    Position position;

  public:
    LogEntry( std::string _message, const Position _position )
      : message( _message ), position( _position ) {}

    std::string getMessage() const {
      boost::format compound( "  %1% :: %2%" );

      return ( compound % position.getPosition() % message ).str();
    }
};

static std::queue<LogEntry> warningsLog;

static std::queue<LogEntry> errorsLog;

bool emptyWarningsLog() {
  return warningsLog.empty();
}

bool emptyErrorsLog() {
  return errorsLog.empty();
}

void printWarnings() {
  std::cout << "Kubic encountered the following warnings --" << std::endl;

  while ( !warningsLog.empty() ) {
    std::cout << warningsLog.front().getMessage() << std::endl;
    warningsLog.pop();
  }
}

void printErrors() {
  std::cout << "Kubic encountered the following errors --" << std::endl;

  while ( !errorsLog.empty() ) {
    std::cout << errorsLog.front().getMessage() << std::endl;
    errorsLog.pop();
  }
}

void log( const Severity _severity, const Position _position, const std::string _message ) {
  LogEntry logEntry( _message, _position );

  if ( _severity == Severity::Warning ) {
    warningsLog.push( logEntry );
  } else {
    errorsLog.push( logEntry );
  }
}

void log( const Severity _severity, const Position _position, boost::format& _compound ) {
  log( _severity, _position, _compound.str() );
}

void log( const Severity _severity, const Position _position, boost::format& _compound, const std::string _message ) {
  log( _severity, _position, _compound % _message );
}

template<typename... Arguments>
void log(
  const Severity _severity,
  const Position _position,
  boost::format& _compound,
  const std::string _message,
  Arguments... _arguments
) {
  log( _severity, _position, _compound % _message, _arguments... );
}

template<typename... Arguments>
void log(
  const Severity _severity,
  const Position _position,
  boost::format& _compound,
  const ValueType _type,
  Arguments... _arguments
) {
  log( _severity, _position, _compound % translateFromValueType( _type ), _arguments... );
}

template<typename... Arguments>
void log(
  const Severity _severity,
  const Position _position,
  const std::string _message,
  const std::string _append,
  Arguments... _arguments
) {
  log( _severity, _position, boost::format( _message ) % _append, _arguments... );
}

template<typename... Arguments>
void log(
  const Severity _severity,
  const Position _position,
  const std::string _message,
  const ValueType _type,
  Arguments... _arguments
) {
  log( _severity, _position, boost::format( _message ) % translateFromValueType( _type ), _arguments... );
}

template<typename... Arguments>
void log( const Severity _severity, const Position _position, const std::string _message, Arguments... _arguments ) {
  log( _severity, _position, boost::format( _message ), _arguments... );
}

#endif
