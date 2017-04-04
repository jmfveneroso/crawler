#include "logger.hpp"
#include "config.h"
#include <iostream>

namespace Crawler {

Logger::Logger() {
  log_file_ = fopen("crawler.log", "wb+");
}

Logger::~Logger() {
  fclose(log_file_);
}

void Logger::Log(const std::string& msg, bool only_on_verbose) {
  if (only_on_verbose && !VERBOSE) return;

  mtx_.lock();
#ifdef LOG_TO_STDOUT
  std::cout << msg << "\n" << std::flush;
#else
  char buffer[1024];
  size_t size = (msg.size() > 1024) ? 1024 : msg.size();
  std::strncpy(buffer, msg.c_str(), size);
  buffer[size - 1] = '\n';
  if (fwrite(buffer, sizeof(char), size, log_file_) != size)
    throw new std::runtime_error("Error writing to priority file.");
  fflush(log_file_);
#endif
  mtx_.unlock();
}

} // End of namespace.
