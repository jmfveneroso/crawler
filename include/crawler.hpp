/**
 * Crawler class
 * @author: jmfveneroso@gmail.com.br
 */

#ifndef __CRAWLER_HPP__
#define __CRAWLER_HPP__

#include <thread>
#include <mutex>
#include "logger.hpp"
#include "fetcher.hpp"
#include "storage.hpp"

#define NUM_THREADS 64

namespace Crawler {

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
  Fetcher fetchers_[NUM_THREADS];
  std::thread threads_[NUM_THREADS];
  std::thread url_fetcher_thread_;
  std::thread delayed_urls_thread_;

  bool fetching_urls_in_disk_ = false;
  size_t fetched_urls_num_ = 0;

  void FetchUrlsFromDisk();
  void FetchPagesAsync(int);
  void ProcessDelayedUrls();

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
