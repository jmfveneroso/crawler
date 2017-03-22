/**
 * Logger class
 * @author: jmfveneroso@gmail.com.br
 */

#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <mutex>
#include <string>

namespace Crawler {

class ILogger {
 public:
  virtual ~ILogger() {}
  virtual void Log(const std::string& msg) = 0;
};

class Logger : public ILogger {
  std::mutex mtx_;

 public:
  Logger();
  
  void Log(const std::string& msg);
};

} // End of namespace.

#endif
