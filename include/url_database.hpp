/**
 * URL Database
 * @author: jmfveneroso@gmail.com.br
 */

#ifndef __URL_DATABASE_HPP__
#define __URL_DATABASE_HPP__

#include <thread>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <map>
#include "logger.hpp"
#include "config.h"

#define TABLE_SIZE 1000000

namespace Crawler {

using namespace std::chrono;

struct Entry {
  bool occupied;
  char url[256];
  system_clock::time_point timestamp;
};

class IUrlDatabase {
 public:
  virtual ~IUrlDatabase() {}
  virtual bool Open(const char*, bool overwrite = false) = 0;
  virtual bool Close() = 0;
  virtual bool Put(const std::string&, system_clock::time_point) = 0;
  virtual bool Get(const std::string&, Entry*) = 0;
};

class UrlDatabase : public IUrlDatabase {
  std::shared_ptr<ILogger> logger_;

  FILE* db_file_;
  size_t table_size_;
  size_t header_size_;
  std::map<std::string, system_clock::time_point> urls_;

  size_t GetHash(std::string const&) const;
  bool Probe(size_t, const std::string&, Entry*);
  bool ReadEntry(size_t, Entry*);

 public:
  UrlDatabase(std::shared_ptr<ILogger> logger);
  ~UrlDatabase();

  bool Open(const char*, bool overwrite = false);
  bool Close();
  bool Put(const std::string&, system_clock::time_point);
  bool Get(const std::string&, Entry*);
};

} // End of namespace.

#endif
