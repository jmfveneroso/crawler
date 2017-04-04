/**
 * Crawler class
 * @author: jmfveneroso@gmail.com.br
 */

#ifndef __CRAWLER_HPP__
#define __CRAWLER_HPP__

#include <thread>
#include <mutex>
#include <chrono>
#include "logger.hpp"
#include "fetcher.hpp"
#include "storage.hpp"
#include "scheduler.hpp"
#include "config.h"

#define NUM_THREADS 512

namespace Crawler {

using namespace std::chrono;

class ICrawler {
 public:
  virtual ~ICrawler() {}
  virtual void Start(const char*, const char*, size_t max_urls = 0) = 0;
  virtual void TestNumberOfThreads(const char*, const char*) = 0;
};

class Crawler : public ICrawler {
  std::shared_ptr<ILogger> logger_;
  std::shared_ptr<IScheduler> scheduler_;
  std::shared_ptr<IUrlDatabase> url_database_;
  std::shared_ptr<IUrlPriorityList> url_priority_list_;
  std::shared_ptr<IFetcher> fetcher_;
  std::shared_ptr<IStorage> storage_;

  static Crawler* instance_;
  Fetcher fetchers_[NUM_THREADS];
  std::thread threads_[NUM_THREADS];
  std::thread print_info_thread_;
  std::mutex mtx_;
  std::mutex print_mtx_;
  bool terminate_ = false;
  int num_threads_ = NUM_THREADS;

  // Control variables.
  system_clock::time_point start_time_;
  size_t max_urls_ = 0;

  // Sample time and size are used to calculate the current crawl speed.
  size_t sample_size_ = 0;
  system_clock::time_point sample_time_;

  bool empty_db_file_ = false;
  size_t fetched_urls_num_ = 0;
  size_t failed_urls_num_ = 0;
  size_t bytes_written_ = 0;
  size_t total_time_ = 0;
  size_t registered_urls_ = 0;
  size_t pages_written_ = 0;
  size_t file_size_ = 0;
  size_t total_urls_ = 0;

  bool Terminate();
  void BlockSignals();
  void FetchPagesAsync(int);
  void ProcessDelayedUrls();
  void PrintInfoCallback();
  void PrintInfo();
  void Seed();

 public:
  Crawler(
    std::shared_ptr<ILogger>,
    std::shared_ptr<IScheduler>,
    std::shared_ptr<IUrlDatabase>,
    std::shared_ptr<IUrlPriorityList>,
    std::shared_ptr<IStorage>
  );

  void TestNumberOfThreads(const char*, const char*);
  void Start(const char*, const char*, size_t max_urls = 0);
  static void SignalHandler(int);
};

} // End of namespace.

#endif
