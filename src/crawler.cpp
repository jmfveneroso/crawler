#include <iostream>
#include "crawler.hpp"

namespace Crawler {

Crawler::Crawler(
  std::shared_ptr<ILogger> logger,
  std::shared_ptr<IScheduler> scheduler,
  std::shared_ptr<IUrlDatabase> url_database,
  std::shared_ptr<IUrlPriorityList> url_priority_list,
  std::shared_ptr<IStorage> storage
) : logger_(logger),
    scheduler_(scheduler),
    url_database_(url_database),
    url_priority_list_(url_priority_list),
    storage_(storage) {
}

void Crawler::FetchUrlsFromDisk() {
  // while (true) {
    // mtx_.lock();
    // if (!fetching_urls_in_disk_) {
    //   std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //   mtx_.unlock();
    //   continue;
    // }
    // mtx_.unlock();
 
    // bool wait = false;
    // for (int i = 0; i < NUM_THREADS; ++i) {
    //   if (fetchers_[i].state() != IDLE) {
    //     wait = true;
    //     break;
    //   }
    // }

    // // Flush all files. Wait for all fetchers to finish downloading
    // // and writing the html files before commiting the cursor positions.
    // if (wait) {
    //   std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //   continue;
    // }

    mtx_.lock();
    url_priority_list_->FetchBlock();
    fetching_urls_in_disk_ = false; 
    mtx_.unlock();
  // }
}

void Crawler::FetchPagesAsync(int fetcher_pos) {
  while (true) {
    mtx_.lock();
    // bool skip = fetching_urls_in_disk_;

    // if (skip) {
    //   logger_->Log("Waiting 1000ms.");
    //   std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //   mtx_.unlock();
    //   continue;
    // }
 
    std::string url;
    if (!scheduler_->GetNextUrl(&url)) {
      // FetchUrlsFromDisk();
      // fetching_urls_in_disk_ = true;
#ifndef IN_MEMORY
      url_priority_list_->FetchBlock();
#endif
      mtx_.unlock();
      continue;
    }
    mtx_.unlock();

    logger_->Log(std::string("Fetching ") + url + " for " + std::to_string(fetcher_pos));

    WebPage web_page = fetchers_[fetcher_pos].GetWebPage(url); 

    mtx_.lock();
    for (auto link : web_page.links) {
      scheduler_->RegisterUrlAsync(link);
    }
    mtx_.unlock();

    fetchers_[fetcher_pos].set_state(FETCHING);
    if (fetchers_[fetcher_pos].state() == FAILED || web_page.html.size() == 0) {
      logger_->Log(std::string("[FAILED] ") + url + ".");
    } else {
      // if (fetched_urls_num_ >= 500) {
      //   logger_->Log("Fetched urls: " + std::to_string(fetched_urls_num_));
      //   return;
      // }
      ++fetched_urls_num_;
      storage_->Write(url, web_page.html);
    }
    fetchers_[fetcher_pos].set_state(IDLE);

    std::string msg = "Fetched urls: ";
    system_clock::time_point now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now - now_);
    msg += std::to_string(fetched_urls_num_) + " in " + std::to_string(ms.count()) + " ms.";
    logger_->Log(msg);
  }
}

void Crawler::ProcessDelayedUrls() {
  while (true) {
    if (!scheduler_->ProcessDelayedQueue()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    if (fetched_urls_num_ > 100) { return; }
  }
}

void Crawler::Start() {
  now_ = system_clock::now();
  storage_->Open("build/html_pages", true);

#ifndef IN_MEMORY
  url_database_->Open("build/db", true);
  url_priority_list_->Open("build/p_list_", true),
#endif

  scheduler_->RegisterUrlAsync("https://www.bcb.gov.br/pec/GCI/PORT/readout/R20161230.pdf");
  // scheduler_->RegisterUrlAsync("globo.com");
  // scheduler_->RegisterUrlAsync("yahoo.com");
  scheduler_->RegisterUrlAsync("jovemnerd.com.br");
  // scheduler_->RegisterUrlAsync("uol.com.br");

  for (int i = 0; i < NUM_THREADS; ++i) {
    threads_[i] = std::thread(&Crawler::FetchPagesAsync, this, i);
  }
  delayed_urls_thread_ = std::thread(&Crawler::ProcessDelayedUrls, this);

  // url_fetcher_thread_ = std::thread(&Crawler::FetchUrlsFromDisk, this);

  for (int i = 0; i < NUM_THREADS; ++i) {
    threads_[i].join();
  }
  // url_fetcher_thread_.join();
  delayed_urls_thread_.join();

  storage_->Close();

#ifndef IN_MEMORY
  url_database_->Close();
  url_priority_list_->Close();
#endif
}

} // End of namespace.
