#include "storage.hpp"
#include <iostream>

namespace Crawler {

Storage::Storage(std::shared_ptr<ILogger> logger)
  : logger_(logger) {
}

Storage::~Storage() {
  fclose(output_file_);
}

void Storage::Open(const std::string& filename, bool overwrite) {
  if (overwrite) {
    output_file_ = fopen(filename.c_str(), "wb+");
  } else {
    output_file_ = fopen(filename.c_str(), "rb+");
  }
  fseeko(output_file_, 0, SEEK_END);
}

void Storage::StripSeparators(std::string& html) {
  for (size_t i = 0; i < html.size(); ++i) {
    if (html[i] == '|') html[i] = ' ';
  }
}

size_t Storage::Write(const std::string& url, std::string& html) {
  StripSeparators(html);

  std::string buffer;
  buffer = url;
  buffer += "|";
  buffer += html;
  buffer += "|||";

  char *c = const_cast<char*>(buffer.c_str());

  mtx_.lock();
  if (fwrite(c, 1, buffer.size(), output_file_) == 0) {
    mtx_.unlock();
    throw 1;
  }
  mtx_.unlock();

  logger_->Log("Wrote " + std::to_string(buffer.size()) + "bytes.");
  return buffer.size();
}

void Storage::Close() {
  fclose(output_file_);
  output_file_ = NULL;
}

} // End of namespace.
