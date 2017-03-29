#include <iostream>
#include <sstream>
#include <iomanip>
#include "crawler.hpp"

namespace Crawler {

Crawler* Crawler::instance_ = 0;

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
  while (max_urls_ > 0 && fetched_urls_num_ < max_urls_) {
    mtx_.lock();
    std::string url;
    if (!scheduler_->GetNextUrl(&url)) {
#ifndef IN_MEMORY_PRIORITY_LIST
      if (!empty_db_file_) {
        if (url_priority_list_->FetchBlock() == 0) empty_db_file_ = true;
      }
#endif
      mtx_.unlock();
      continue;
    }
    mtx_.unlock();

#ifdef VERBOSE
    logger_->Log(std::string("Fetching ") + url + " for " + std::to_string(fetcher_pos));
#endif

    fetchers_[fetcher_pos].set_state(FETCHING);
    WebPage *web_page = fetchers_[fetcher_pos].GetWebPage(url); 

    mtx_.lock();
    if (fetchers_[fetcher_pos].state() == FAILED || web_page->html.size() == 0) {
#ifdef VERBOSE
      logger_->Log(std::string("[FAILED] ") + url);
#endif
      ++failed_urls_num_;
      mtx_.unlock();
      continue;
    } else {
      for (auto link : web_page->links) {
        if (scheduler_->RegisterUrlAsync(link)) {
          empty_db_file_ = false;
          ++registered_urls_;
        }
      }
      mtx_.unlock();

      bytes_written_ += storage_->Write(url, web_page->html);
      ++fetched_urls_num_;
      ++sample_size_;
    }
    delete web_page;
    fetchers_[fetcher_pos].set_state(IDLE); 

#ifdef VERBOSE
    mtx_.lock();
    std::string msg = "Fetched urls: ";
    system_clock::time_point now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now - now_);
    msg += std::to_string(fetched_urls_num_) + " in " + std::to_string(ms.count()) + " ms.";
    logger_->Log(msg);
    mtx_.unlock();
#endif
  }
}

void Crawler::ProcessDelayedUrls() {
  while (max_urls_ > 0 && fetched_urls_num_ < max_urls_) {
    if (!scheduler_->ProcessDelayedQueue()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  }
}

void Crawler::PrintInfo(bool print_all) {
  system_clock::time_point now = system_clock::now();
  auto ms = duration_cast<milliseconds>(now - now_);
  logger_->Log("==================================================");
  logger_->Log("Elapsed time (ms): " + std::to_string(ms.count())        + "");
  logger_->Log("Webpages fetched: "  + std::to_string(fetched_urls_num_) + "");
  logger_->Log("Failed urls: "       + std::to_string(failed_urls_num_)  + "");
  logger_->Log("Bytes written: "     + std::to_string(bytes_written_)    + "");
  logger_->Log("Registered urls: "   + std::to_string(registered_urls_)  + "");

  std::stringstream ss;
  auto cur_ms = duration_cast<milliseconds>(now - sample_time_);
  double pps_cur = ((double) sample_size_ / ((cur_ms.count() / 1000) + 1));
  ss << std::fixed << std::setprecision(2) << pps_cur;
  logger_->Log("Pages per second (current): " + ss.str() + " pages/s");
  sample_size_ = 0;
  sample_time_ = now;

  ss.str(std::string());
  double pages_per_second = ((double) fetched_urls_num_ / ((ms.count() / 1000) + 1));
  ss << std::fixed << std::setprecision(2) << pages_per_second;
  logger_->Log("Pages per second (overall): " + ss.str() + " pages/s");

  if (print_all) {
    mtx_.lock();
#ifndef IN_MEMORY
    double percentage = url_database_->CheckEmptyBuckets();
    ss.str(std::string());
    ss << std::fixed << std::setprecision(2) << 100 - percentage * 100;
    logger_->Log("Hash table (empty): " + ss.str() + "%");
#endif

#ifndef IN_MEMORY_PRIORITY_LIST
    size_t total_urls = 0;
    for (int i = 0; i < PRIORITY_LEVELS; ++i) {
      size_t num_urls = url_priority_list_->GetNumUrlsAtPriorityLevel(i);
      logger_->Log("URLs at priority level " + std::to_string(i) + ": " + std::to_string(num_urls) + "");
      total_urls += num_urls;
    }
    logger_->Log("URLs total: " + std::to_string(total_urls) + "");
#endif

#if !defined IN_MEMORY_PRIORITY_LIST && !defined IN_MEMORY
    size_t pages_written = 0;
    size_t file_size = 0;
    storage_->CheckStorageFile(&file_size, &pages_written);
    logger_->Log("Total pages written: " + std::to_string(pages_written) + "");
    logger_->Log("File size: " + std::to_string(file_size) + " bytes");

    size_t avg_size = 0;
    double growth_ratio = 0;
    if (pages_written != 0) {
      avg_size = (file_size / pages_written) - 4;
      growth_ratio = ((double) total_urls / pages_written);
    }

    logger_->Log("Average page size: " + std::to_string(avg_size) + " bytes");

    ss.str(std::string());
    ss << std::fixed << std::setprecision(2) << growth_ratio;
    logger_->Log("Growth ratio: " + ss.str() + " links/page");
#endif
    mtx_.unlock();
  }
}

void Crawler::PrintInfoCallback() {
  static int counter = 0;
  while (max_urls_ > 0 && fetched_urls_num_ < max_urls_) {
    PrintInfo(counter % 20 == 0); // Print the complete information every 10 ticks.
    ++counter;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  }
}

void Crawler::SignalHandler(int signum) {
   instance_->mtx_.lock();
   instance_->logger_->Log("Interrupt signal (" + std::to_string(signum) + ") received.\n");

   // Prevent storage from writing anything else.
   instance_->storage_->StopWriting();

   // Commit cursor positions.
   instance_->url_priority_list_->FetchBlock();
   // instance_->PrintInfo(true);
   exit(signum);  
}

void Crawler::Start() {
  std::cout << "Started" << std::endl;
  instance_ = this;
  num_threads_ = NUM_THREADS;
  now_ = system_clock::now();
  bytes_written_ = 0;
  fetched_urls_num_ = 0;
  empty_db_file_ = false;
  total_time_ = 0;
  sample_time_ = system_clock::now();
  scheduler_->ClearDelayedQueue();

  storage_->Open("/mnt/hd0/joao_test/html_pages", false);
  
#ifndef IN_MEMORY
  url_database_->Open("/mnt/hd0/joao_test/db", false);
#endif
#ifndef IN_MEMORY_PRIORITY_LIST
  url_priority_list_->Open("/mnt/hd0/joao_test/p_list_", false),
#endif
 
  scheduler_->RegisterUrlAsync("noticias.terra.com.br");
 
  delayed_urls_thread_ = new std::thread(&Crawler::ProcessDelayedUrls, this);
  print_info_thread_ = new std::thread(&Crawler::PrintInfoCallback, this);
  for (int i = 0; i < num_threads_; ++i) {
    threads_[i] = new std::thread(&Crawler::FetchPagesAsync, this, i);
  }

  for (int i = 0; i < num_threads_; ++i) {
    threads_[i]->join();
    delete threads_[i];
  }
  delayed_urls_thread_->join();
  delete delayed_urls_thread_;

  // std::cout << num_threads_ << ", " << total_time_ << ", " << bytes_written_ << std::endl;
  storage_->Close();

#ifndef IN_MEMORY
  url_database_->Close();
#endif
#ifndef IN_MEMORY_PRIORITY_LIST
  url_priority_list_->Close();
#endif

  std::cout << "Finished." << std::endl;
}

} // End of namespace.
