#include "storage.hpp"
#include <iostream>

namespace Crawler {

Storage::Storage(std::shared_ptr<ILogger> logger)
  : logger_(logger) {
}

Storage::~Storage() {
  fclose(output_file_);
}

void Storage::Open(const char* filename, bool overwrite) {
  stopped_ = false;
  bool file_exists = access(filename, F_OK) != -1;
  if (!file_exists || overwrite) {
    output_file_ = fopen(filename, "wb+");
    if (output_file_ == NULL) 
      throw new std::runtime_error("Error opening output file.");

    char buffer[] = "|||";
    fwrite(buffer, 1, 3, output_file_);
  } else {
    output_file_ = fopen(filename, "rb+");
    if (output_file_ == NULL) 
      throw new std::runtime_error("Error opening output file.");
  }
  fseeko(output_file_, 0, SEEK_END);
}

void Storage::StripSeparators(std::string& html) {
  for (size_t i = 0; i < html.size(); ++i)
    if (html[i] == '|') html[i] = ' ';
}

size_t Storage::Write(std::string& url, std::string& html) {
  if (html.size() == 0 || url.size() == 0) return 0;

  if (html[0] != '<') {
#ifdef VERBOSE
    logger_->Log(url + " is not html.");
#endif
    return 0;
  }

  StripSeparators(url);
  StripSeparators(html);

  std::string buffer;
  buffer = url;
  buffer += "|";
  buffer += html;
  buffer += "|||";

  char *c = const_cast<char*>(buffer.c_str());

  mtx_.lock();
  if (stopped_) { 
    mtx_.unlock();
    return 0;
  }

  if (fwrite(c, 1, buffer.size(), output_file_) == 0) {
    mtx_.unlock();
    throw 1;
  }
  mtx_.unlock();

#ifdef VERBOSE
  logger_->Log("Wrote " + url + " as " + std::to_string(buffer.size()) + "bytes.");
#endif
  return buffer.size();
}

void Storage::Close() {
  fclose(output_file_);
  output_file_ = NULL;
}

void Storage::StopWriting() {
  mtx_.lock();
  stopped_ = true;
  mtx_.unlock();
}

void Storage::CheckStorageFile(size_t* file_size, size_t* urls) {
  fseeko(output_file_, 3, SEEK_SET);

  size_t pipes = 0;
  *urls = 0;
  *file_size = 3;

  char buffer[1024];
  size_t num_read = 0;
  while ((num_read = fread(buffer, sizeof(char), 1024, output_file_)) != 0) {
    for (size_t i = 0; i < num_read; ++i) {
      if (buffer[i] == '|') {
        ++pipes;
        continue;
      }

      if (pipes == 1) {
        *urls += 1;
      }
      pipes = 0;
    }
    *file_size += num_read;
  }
}

} // End of namespace.
