/**
 * Storage class
 * @author: jmfveneroso@gmail.com.br
 */

#ifndef __STORAGE_HPP__
#define __STORAGE_HPP__

#include <memory>
#include <stdio.h>
#include "logger.hpp"

namespace Crawler {

class IStorage {
 public:
  virtual ~IStorage() {}
  virtual void Write(const std::string&, const std::string&) = 0;
};

class Storage : public IStorage {
  std::shared_ptr<ILogger> logger_;
  FILE* output_file_;

 public:
  Storage(std::shared_ptr<ILogger>);
  ~Storage();

  void Write(const std::string&, const std::string&);
};

} // End of namespace.

#endif
