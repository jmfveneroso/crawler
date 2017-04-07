/**
 * URL Priority List
 * @author: jmfveneroso@gmail.com.br
 */

#ifndef __URL_PRIORITY_LIST_HPP__
#define __URL_PRIORITY_LIST_HPP__

#include <string>
#include <vector>
#include <queue>
#include <memory>
#include "logger.hpp"
#include "config.h"

#define PRIORITY_LEVELS 10
#define MAX_PRIORITY_LEVEL 9
#define MAX_UNCRAWLED_URLS_STORED 5000 // In each queue.
#define MAX_WAIT_TIME 200000 // 200 seconds.

namespace Crawler {

using namespace std::chrono;

struct UncrawledUrl {
  std::string url;
  system_clock::time_point scheduled_time;
  UncrawledUrl(std::string, system_clock::time_point);
};

struct UncrawledUrlComparator {
  // This function returns true when a should go before b.
  bool operator() (const UncrawledUrl& a, const UncrawledUrl& b) const {
    auto ms = duration_cast<milliseconds>(b.scheduled_time - a.scheduled_time);
    return ms.count() < 0; // This is true when a is scheduled earlier than b.
  }
};

// Alias for typing convenience.
using PriorityQueue = std::priority_queue<
  UncrawledUrl, std::vector<UncrawledUrl>, UncrawledUrlComparator
>;

class IUrlPriorityList {
 public:
  virtual ~IUrlPriorityList() {}
  virtual size_t num_uncrawled_urls() = 0;
  virtual size_t num_registered_urls() = 0;
  virtual int GetPriority(const std::string&) = 0;
  virtual bool Push(const std::string&, const system_clock::time_point&) = 0;
  virtual bool Pop(std::string*) = 0;
  virtual size_t GetNumUrlsAtPriorityLevel(const int&) = 0;
  virtual void Clear() = 0;
};

class UrlPriorityList : public IUrlPriorityList {
  std::shared_ptr<ILogger> logger_;
  PriorityQueue uncrawled_urls_[MAX_PRIORITY_LEVEL + 1];
  size_t num_uncrawled_urls_;
  size_t num_registered_urls_;

 public:
  UrlPriorityList(std::shared_ptr<ILogger> logger);
  ~UrlPriorityList();

  size_t num_uncrawled_urls() { return num_uncrawled_urls_; }
  size_t num_registered_urls() { return num_registered_urls_; }
  int GetPriority(const std::string&);
  bool Push(const std::string&, const system_clock::time_point&);
  bool Pop(std::string*);
  size_t GetNumUrlsAtPriorityLevel(const int&);
  void Clear();
};

} // End of namespace.

#endif
