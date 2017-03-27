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
#include "config.h"

#define NUM_THREADS 512
#define NUM_URLS 1000

namespace Crawler {

using namespace std::chrono;

class ICrawler {
 public:
  virtual ~ICrawler() {}
  virtual void Start() = 0;
};

class Crawler : public ICrawler {
  std::shared_ptr<ILogger> logger_;
  std::shared_ptr<IScheduler> scheduler_;
  std::shared_ptr<IUrlDatabase> url_database_;
  std::shared_ptr<IUrlPriorityList> url_priority_list_;
  std::shared_ptr<IFetcher> fetcher_;
  std::shared_ptr<IStorage> storage_;

  std::mutex mtx_;
  system_clock::time_point now_;

  Fetcher fetchers_[NUM_THREADS];
  std::thread* threads_[NUM_THREADS];
  std::thread delayed_urls_thread_;

  bool empty_db_file_ = false;
  size_t fetched_urls_num_ = 0;
  size_t bytes_written_ = 0;
  size_t total_time_ = 0;

  void FetchPagesAsync(int);
  void ProcessDelayedUrls();

  int num_threads_ = 1;

 public:
  Crawler(
    std::shared_ptr<ILogger>,
    std::shared_ptr<IScheduler>,
    std::shared_ptr<IUrlDatabase>,
    std::shared_ptr<IUrlPriorityList>,
    std::shared_ptr<IStorage>
  );

  void Start();
};

} // End of namespace.

#endif
