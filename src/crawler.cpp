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

void Crawler::FetchPagesAsync(int fetcher_pos) {
  while (true) {
    mtx_.lock();
    if (fetched_urls_num_ >= NUM_URLS) {  
      mtx_.unlock();
      return; 
    }

    std::string url;
    if (!scheduler_->GetNextUrl(&url)) {
#ifndef IN_MEMORY_PRIORITY_LIST
      if (!empty_db_file_) {
        if (url_priority_list_->FetchBlock() == 0) empty_db_file_ = true;
      }
#endif
      mtx_.unlock();
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      continue;
    }
    mtx_.unlock();

    logger_->Log(std::string("Fetching ") + url + " for " + std::to_string(fetcher_pos));

    fetchers_[fetcher_pos].set_state(FETCHING);
    WebPage *web_page = fetchers_[fetcher_pos].GetWebPage(url); 
    if (fetchers_[fetcher_pos].state() == FAILED || web_page->html.size() == 0) {
      logger_->Log(std::string("[FAILED] ") + url);
      continue;
    } else {
      mtx_.lock();
      for (auto link : web_page->links) {
        if (scheduler_->RegisterUrlAsync(link)) {
          empty_db_file_ = false;
        }
      }
      mtx_.unlock();

      ++fetched_urls_num_;
      if (fetched_urls_num_ <= NUM_URLS) {
        bytes_written_ += storage_->Write(url, web_page->html);
      }
    }
    delete web_page;

    fetchers_[fetcher_pos].set_state(IDLE); 

    std::string msg = "Fetched urls: ";
    system_clock::time_point now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now - now_);
    msg += std::to_string(fetched_urls_num_) + " in " + std::to_string(ms.count()) + " ms.";
    logger_->Log(msg);

    if (fetched_urls_num_ >= NUM_URLS) {
      if (total_time_ == 0) total_time_ = ms.count();
    }
  }
}

void Crawler::ProcessDelayedUrls() {
  while (true) {
    if (fetched_urls_num_ >= NUM_URLS) return;
    if (!scheduler_->ProcessDelayedQueue()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  }
}

void Crawler::Start() {
  std::cout << "Started" << std::endl;
  for (num_threads_ = 512; num_threads_ <= 512; num_threads_ += 8) {
    now_ = system_clock::now();
    bytes_written_ = 0;
    fetched_urls_num_ = 0;
    empty_db_file_ = false;
    total_time_ = 0;
    scheduler_->ClearDelayedQueue();

    storage_->Open("/mnt/hd0/joao_test/html_pages", true);
  
#ifndef IN_MEMORY
    url_database_->Open("/mnt/hd0/joao_test/db", true);
#endif
#ifndef IN_MEMORY_PRIORITY_LIST
    url_priority_list_->Open("/mnt/hd0/joao_test/p_list_", true),
#endif
  
    scheduler_->RegisterUrlAsync("noticias.terra.com.br");
 
    delayed_urls_thread_ = new std::thread(&Crawler::ProcessDelayedUrls, this);
    for (int i = 0; i < num_threads_; ++i) {
      threads_[i] = new std::thread(&Crawler::FetchPagesAsync, this, i);
    }

    for (int i = 0; i < num_threads_; ++i) {
      threads_[i]->join();
      delete threads_[i];
    }
    delayed_urls_thread_->join();
    delete delayed_urls_thread_;

    std::cout << num_threads_ << ", " << total_time_ << ", " << bytes_written_ << std::endl;
    storage_->Close();

#ifndef IN_MEMORY
    url_database_->Close();
#endif
#ifndef IN_MEMORY_PRIORITY_LIST
    url_priority_list_->Close();
#endif
  }

  std::cout << "Finished." << std::endl;
}

} // End of namespace.
