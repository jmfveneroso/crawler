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

#define NUM_THREADS 60

namespace Crawler {

class ICrawler {
 public:
  virtual ~ICrawler() {}
  virtual void Start() = 0;
};

class Crawler : public ICrawler {
  std::shared_ptr<ILogger> logger_;
  std::shared_ptr<IScheduler> scheduler_;
  std::shared_ptr<IFetcher> fetcher_;
  std::shared_ptr<IStorage> storage_;

  std::mutex scheduler_mtx_;
  std::mutex write_mtx_;
  std::thread threads_[NUM_THREADS];

  void FetchPagesAsync();

 public:
  Crawler(
    std::shared_ptr<ILogger>,
    std::shared_ptr<IScheduler>,
    std::shared_ptr<IFetcher>,
    std::shared_ptr<IStorage>
  );

  void Start();
};

} // End of namespace.

#endif
