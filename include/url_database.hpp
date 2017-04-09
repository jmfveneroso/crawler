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

#define TABLE_SIZE 30000000

namespace Crawler {

using namespace std::chrono;

struct Entry {
  bool occupied;
  char url[257];
  system_clock::time_point timestamp;

  Entry();
  Entry(bool, std::string, system_clock::time_point);
};

class IUrlDatabase {
 public:
  virtual ~IUrlDatabase() {}
  virtual size_t num_unique_urls() = 0;
  virtual bool Open(const char*, bool overwrite = false) = 0;
  virtual bool Close() = 0;
  virtual bool Put(const std::string&, system_clock::time_point) = 0;
  virtual bool Get(const std::string&, Entry*) = 0;
  virtual double CheckEmptyBuckets() = 0;
  virtual void Clear() = 0;
  virtual void set_table_size(size_t table_size) = 0;
};

class UrlDatabase : public IUrlDatabase {
  std::shared_ptr<ILogger> logger_;

  FILE* db_file_;
  size_t table_size_;
  size_t header_size_;
  size_t num_unique_urls_ = 0;
  std::map<std::string, system_clock::time_point> urls_;

  size_t GetHash(std::string const&) const;
  bool Probe(size_t, const std::string&, Entry*);
  bool ReadEntry(size_t, Entry*);

  Entry buffer_;

 public:
  UrlDatabase(std::shared_ptr<ILogger> logger);
  ~UrlDatabase();

  size_t num_unique_urls() { return num_unique_urls_; }
  bool Open(const char*, bool overwrite = false);
  bool Close();
  bool Put(const std::string&, system_clock::time_point);
  bool Get(const std::string&, Entry*);
  double CheckEmptyBuckets();
  void Clear();
  void set_table_size(size_t table_size) { table_size_ = table_size; }
};

} // End of namespace.

#endif
