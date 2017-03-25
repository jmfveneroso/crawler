/**
 * Storage class
 * @author: jmfveneroso@gmail.com.br
 */

#ifndef __STORAGE_HPP__
#define __STORAGE_HPP__

#include <memory>
#include <stdio.h>
#include <mutex>
#include "logger.hpp"

namespace Crawler {

class IStorage {
 public:
  virtual ~IStorage() {}
  virtual void Write(const std::string&, std::string&) = 0;
  virtual void Open(const std::string&, bool) = 0;
  virtual void Close() = 0;
};

class Storage : public IStorage {
  std::shared_ptr<ILogger> logger_;

  std::mutex mtx_;
  FILE* output_file_;

  void StripSeparators(std::string&);

 public:
  Storage(std::shared_ptr<ILogger>);
  ~Storage();

  void Open(const std::string&, bool);
  void Close();
  void Write(const std::string&, std::string&);
};

} // End of namespace.

#endif
