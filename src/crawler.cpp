#include <iostream>
#include "crawler.hpp"

namespace Crawler {

Crawler::Crawler(
  std::shared_ptr<ILogger> logger,
  std::shared_ptr<IScheduler> scheduler,
  std::shared_ptr<IFetcher> fetcher,
  std::shared_ptr<IStorage> storage
) : logger_(logger),
    scheduler_(scheduler),
    fetcher_(fetcher),
    storage_(storage) {
}

void Crawler::FetchPagesAsync() {
  while (true) {
    scheduler_mtx_.lock();
    std::string url = scheduler_->GetNextUrl();
    scheduler_mtx_.unlock();

    while (url.empty()) {
      std::this_thread::sleep_for(std::chrono::seconds(1));

      scheduler_mtx_.lock();
      url = scheduler_->GetNextUrl();
      scheduler_mtx_.unlock();
    }

    logger_->Log(std::string("Fetching ") + url);

    WebPage web_page = fetcher_->GetWebPage(url); 

    scheduler_mtx_.lock();
    // Lower priority.
    for (auto url : web_page.inbound_links) {
      scheduler_->AddUrl(url);
    }

    // Higher priority.
    for (auto url : web_page.outbound_links) {
      scheduler_->AddUrl(url); 
    }
    scheduler_mtx_.unlock();

    write_mtx_.lock();
    storage_->Write(url, web_page.html);
    write_mtx_.unlock();
  }
}

void Crawler::Start() {
  scheduler_->AddUrl("globo.com");

  for (int i = 0; i < NUM_THREADS; ++i) {
    threads_[i] = std::thread(&Crawler::FetchPagesAsync, this);
  }

  for (int i = 0; i < NUM_THREADS; ++i) {
    threads_[i].join();
  }
}

} // End of namespace.
