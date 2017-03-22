#include "storage.hpp"
#include <iostream>

namespace Crawler {

Storage::Storage(std::shared_ptr<ILogger> logger)
  : logger_(logger) {
  output_file_ = fopen("data", "wb");
}

Storage::~Storage() {
  fclose(output_file_);
}

void Storage::Write(const std::string& url, const std::string& html) {
  std::string buffer;
  buffer = "|||";
  buffer += url;
  buffer += "|";
  buffer += html;
  buffer += "|||";

  char *c = const_cast<char*>(buffer.c_str());
  if (fwrite(c, 1, buffer.size(), output_file_) == 0) {
    std::cout << "ERROR!!!!" << std::endl;
  }

  std::cout << "Wrote " << buffer.size() << "bytes." << std::endl;
}

}; // End of namespace.
