/**
 * Scheduler class
 * @author: jmfveneroso@gmail.com.br
 */

#ifndef __SCHEDULER_HPP__
#define __SCHEDULER_HPP__

#include <thread>
#include <string>
#include <queue>
#include <set>
#include <map>
#include <chrono>
#include <iostream>
#include "logger.hpp"
#include "url_database.hpp"
#include "url_priority_list.hpp"

namespace Crawler {

using namespace std::chrono;

struct DelayedUrl {
  std::string url; 
  system_clock::time_point timestamp;

  DelayedUrl() : url() {
    timestamp = system_clock::time_point();
  }

  DelayedUrl(const std::string& url, system_clock::time_point timestamp) 
    : url(url), timestamp(timestamp) {
  }

  bool operator() (const DelayedUrl& lhs, const DelayedUrl& rhs) const {
    auto ms = duration_cast<milliseconds>(rhs.timestamp - lhs.timestamp);
    return ms.count() > 0;
  }
};

// Aliases for typing convenience.
using PriorityQueue = std::priority_queue<DelayedUrl, std::vector<DelayedUrl>, DelayedUrl>;

class IScheduler {
 public:
  virtual ~IScheduler() {}
  virtual bool RegisterUrlAsync(const std::string&) = 0;
  virtual bool ProcessDelayedQueue() = 0;
  virtual bool GetNextUrl(std::string*) = 0;
  virtual void set_politeness_policy(size_t) = 0;
};

class Scheduler : public IScheduler {
  std::shared_ptr<ILogger> logger_;
  std::shared_ptr<IUrlDatabase> url_database_;
  std::shared_ptr<IUrlPriorityList> url_priority_list_;

  std::mutex mtx_;
  PriorityQueue delayed_queue_;
  std::queue<std::string> ready_queue_;
  long long politeness_policy_ = 1000;

  static std::string TruncateUrl(std::string);
  static std::string GetRootUrl(std::string);
  bool EnforcePolitenessPolicy(const std::string&);
  bool RegisterUrl(const std::string&);

 public:
  Scheduler(
    std::shared_ptr<ILogger> logger,
    std::shared_ptr<IUrlDatabase> url_database_,
    std::shared_ptr<IUrlPriorityList> url_priority_list_
  );

  void set_politeness_policy(size_t time) { politeness_policy_ = time; }
  bool RegisterUrlAsync(const std::string&);
  bool ProcessDelayedQueue();
  bool GetNextUrl(std::string*);
};

} // End of namespace.

#endif
