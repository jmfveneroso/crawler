#include "logger.hpp"
#include "config.h"
#include <iostream>

namespace Crawler {

Logger::Logger() {}

void Logger::Log(const std::string& msg) {
  mtx_.lock();
#ifdef LOG
  std::cout << msg << "\n";
#else
  msg.size();
#endif
  mtx_.unlock();
}

} // End of namespace.
