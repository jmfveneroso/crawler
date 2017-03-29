/**
 * Storage class
 * @author: jmfveneroso@gmail.com.br
 */

#ifndef __STORAGE_HPP__
#define __STORAGE_HPP__

#include <memory>
#include <stdio.h>
#include <mutex>
#include <unistd.h>
#include "logger.hpp"

namespace Crawler {

class IStorage {
 public:
  virtual ~IStorage() {}
  virtual size_t Write(std::string&, std::string&) = 0;
  virtual void Open(const char*, bool) = 0;
  virtual void Close() = 0;
  virtual void StopWriting() = 0;
  virtual void CheckStorageFile(size_t*, size_t*) = 0;
};

class Storage : public IStorage {
  std::shared_ptr<ILogger> logger_;

  std::mutex mtx_;
  FILE* output_file_;
  bool stopped_ = false;

  void StripSeparators(std::string&);

 public:
  Storage(std::shared_ptr<ILogger>);
  ~Storage();

  void Open(const char*, bool);
  void Close();
  size_t Write(std::string&, std::string&);
  void StopWriting();
  void CheckStorageFile(size_t*, size_t*);
};

} // End of namespace.

#endif
