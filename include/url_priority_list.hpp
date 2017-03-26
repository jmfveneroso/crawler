/**
 * URL Priority List
 * @author: jmfveneroso@gmail.com.br
 */

#ifndef __URL_PRIORITY_LIST_HPP__
#define __URL_PRIORITY_LIST_HPP__

#include <thread>
#include <cstring>
#include <queue>
#include <stdio.h>
#include <unistd.h>
#include "logger.hpp"
#include "config.h"

#define PRIORITY_LEVELS 10
#define BLOCK_SIZE 10000

namespace Crawler {

class IUrlPriorityList {
 public:
  virtual ~IUrlPriorityList() {}
  virtual bool Open(const std::string&, bool overwrite = false) = 0;
  virtual bool Push(const std::string&) = 0;
  virtual bool Pop(std::string*) = 0;
  virtual size_t FetchBlock() = 0;
  virtual bool Close() = 0;
};

class UrlPriorityList : public IUrlPriorityList {
  std::shared_ptr<ILogger> logger_;

  size_t fingerprint_ = 0xe1137371bf83af76;
  size_t header_size_;
  FILE* priority_files_[PRIORITY_LEVELS];
  size_t file_cursors_[PRIORITY_LEVELS];
  std::queue<std::string> urls_;
  size_t written_urls_num_;

  void CommitFileCursors();
  int GetPriority(const std::string&);

 public:
  UrlPriorityList(std::shared_ptr<ILogger> logger);
  ~UrlPriorityList();

  bool Open(const std::string&, bool overwrite = false);
  bool Push(const std::string&);
  bool Pop(std::string*);
  size_t FetchBlock();
  bool Close();
};

} // End of namespace.

#endif
