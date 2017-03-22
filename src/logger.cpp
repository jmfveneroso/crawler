#include "logger.hpp"
#include <iostream>

namespace Crawler {

Logger::Logger() {}

void Logger::Log(const std::string& msg) {
  mtx_.lock();
  std::cout << msg << "\n";
  mtx_.unlock();
}

} // End of namespace.
