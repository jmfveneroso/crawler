/**
 * Scheduler class
 * @author: jmfveneroso@gmail.com.br
 */

#ifndef __SCHEDULER_HPP__
#define __SCHEDULER_HPP__

#include <thread>
#include <string>
#include <chrono>
#include <set>
#include "logger.hpp"
#include "url_database.hpp"
#include "url_priority_list.hpp"

namespace Crawler {

using namespace std::chrono;

class IScheduler {
 public:
  virtual ~IScheduler() {}
  virtual bool RegisterUrlAsync(const std::string&) = 0;
  virtual bool GetNextUrl(std::string*) = 0;
  virtual void AddPrivilegedRoot(const std::string&) = 0;
};

class Scheduler : public IScheduler {
  std::shared_ptr<ILogger> logger_;
  std::shared_ptr<IUrlDatabase> url_database_;
  std::shared_ptr<IUrlPriorityList> url_priority_list_;
  std::set<std::string> privileged_roots_;
  std::mutex mtx_;

  static std::string TruncateUrl(std::string);
  static std::string GetRootUrl(std::string);
  bool RegisterUrl(const std::string&);
  bool CheckRootValidity(const std::string&);

 public:
  Scheduler(
    std::shared_ptr<ILogger> logger,
    std::shared_ptr<IUrlDatabase> url_database_,
    std::shared_ptr<IUrlPriorityList> url_priority_list_
  );

  bool RegisterUrlAsync(const std::string&);
  bool GetNextUrl(std::string*);
  void AddPrivilegedRoot(const std::string&);
};

} // End of namespace.

#endif
