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
#include "logger.hpp"

// 2 ^ 32 total size and 2 ^ 16 partition size.
// 8192 hashes.
#define PARTITION_SIZE 1024
#define NUM_PARTITIONS 1024 // 524288

namespace Crawler {

class UrlComparator {
  bool GetUrlWordSize(std::string);

 public:
  bool operator() (std::string, std::string);
};

class IScheduler {
 public:
  virtual ~IScheduler() {}
  virtual std::string GetNextUrl() = 0;
  virtual bool AddUrl(const std::string&) = 0;
};

class Scheduler : public IScheduler {
  std::shared_ptr<ILogger> logger_;
  std::mutex mtx_;
  std::priority_queue<std::string, std::vector<std::string>, UrlComparator> url_queue_;
  std::map<std::string, std::chrono::system_clock::time_point> last_requests_;
  std::thread polite_thread_;
  off_t url_pos_;
  // std::map<size_t, std::string> crawled_urls_;

  FILE* file_;
  FILE* queue_file_;
  long int partition_offset_[NUM_PARTITIONS];

  long int GetPartitionOffset(size_t);
  size_t GetHash(const std::string&) const;
  bool IsUnique(const std::string&);
  void SetUnique(const std::string&);
  bool ShouldWait(const std::string&);
  void WaitPolitelyAsync();
  bool ReadNextBatch();
  void Init();

  std::queue<std::string> waiting_queue_;

  static std::string GetRootUrl(std::string);

 public:
  Scheduler(std::shared_ptr<ILogger> logger);

  std::string GetNextUrl();
  bool AddUrl(const std::string&);
};

}; // End of namespace.

#endif
