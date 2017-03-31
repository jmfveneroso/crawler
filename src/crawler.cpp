#include <iostream>
#include <sstream>
#include <iomanip>
#include <signal.h>
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

bool Crawler::Terminate() {
  if (max_urls_ > 0 && fetched_urls_num_ >= max_urls_) return true;
  return terminate_;
}

void Crawler::FetchPagesAsync(int fetcher_pos) {
  struct sigaction setup_action;
  sigset_t block_mask;

  sigemptyset (&block_mask);
  /* Block other terminal-generated signals while handler runs. */
  sigaddset (&block_mask, SIGINT);
  sigaddset (&block_mask, SIGQUIT);
  setup_action.sa_mask = block_mask;
  setup_action.sa_flags = 0;
  sigaction (SIGTSTP, &setup_action, NULL);

  while (!Terminate()) {
    mtx_.lock();
    std::string url;
    if (!scheduler_->GetNextUrl(&url)) {
#ifndef IN_MEMORY_PRIORITY_LIST
      if (!empty_db_file_) {
        ++disk_requests_;
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
    WebPage web_page;
    fetchers_[fetcher_pos].GetWebPage(url, &web_page); 

    mtx_.lock();
    if (fetchers_[fetcher_pos].state() == FAILED || web_page.html.size() == 0) {
#ifdef VERBOSE
      logger_->Log(std::string("[FAILED] ") + url);
#endif
      ++failed_urls_num_;
      mtx_.unlock();
      continue;
    } else {
      // Stop registering new urls if there are more than 50.000 pages
      // waiting to be crawled.
      bool register_new_urls = (url_priority_list_->unread_urls_num() < 200000);

#ifdef IN_MEMORY_PRIORITY_LIST
      register_new_urls = true;
#endif

      if (register_new_urls) {
        for (auto link : web_page.links) {
          if (scheduler_->RegisterUrlAsync(link)) {
            empty_db_file_ = false;
            ++registered_urls_;
            if (max_urls_ != 0) ++total_urls_;
          }
        }
      }
      mtx_.unlock();

      bytes_written_ += storage_->Write(url, web_page.html);
      if (bytes_written_ == 0) {
#ifdef VERBOSE
        logger_->Log("Url " + url + " was not written to disk.");      
#endif
      } else {
        if (max_urls_ != 0) ++pages_written_;
        ++fetched_urls_num_;
        ++sample_size_;
      }
    }
    fetchers_[fetcher_pos].set_state(IDLE); 

#ifdef VERBOSE
    mtx_.lock();
    std::string msg = "Fetched url " + url + ". Total is ";
    system_clock::time_point now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now - now_);
    msg += std::to_string(fetched_urls_num_) + " in " + std::to_string(ms.count()) + " ms.";
    logger_->Log(msg);
    mtx_.unlock();
#endif
  }
  if (total_time_ == 0) {
    system_clock::time_point now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now - now_); 
    total_time_ = ms.count();
    mtx_.lock();
    storage_->StopWriting();
    mtx_.unlock();
  }
}

void Crawler::ProcessDelayedUrls() {
  while (!Terminate()) {
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
  logger_->Log("Delayed urls: "      + std::to_string(scheduler_->delayed_queue_size())  + "");
  logger_->Log("Ready urls: "        + std::to_string(scheduler_->ready_queue_size())  + "");
  logger_->Log("Disk requests: "     + std::to_string(disk_requests_)  + "");
  disk_requests_ = 0;

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
    total_urls_ = 0;
    for (int i = 0; i < PRIORITY_LEVELS; ++i) {
      size_t num_urls = url_priority_list_->GetNumUrlsAtPriorityLevel(i);
      logger_->Log("URLs at priority level " + std::to_string(i) + ": " + std::to_string(num_urls) + "");
      total_urls_ += num_urls;
    }
    logger_->Log("URLs total: " + std::to_string(total_urls_) + "");
#endif

#if !defined IN_MEMORY_PRIORITY_LIST && !defined IN_MEMORY
    // storage_->CheckStorageFile(&file_size_, &pages_written_);
    // logger_->Log("Total pages written: " + std::to_string(pages_written_) + "");
    // logger_->Log("File size: " + std::to_string(file_size_) + " bytes");

    // size_t avg_size = 0;
    // double growth_ratio = 0;
    // if (pages_written_ != 0) {
    //   avg_size = (file_size_ / pages_written_) - 4;
    //   growth_ratio = ((double) total_urls_ / pages_written_);
    // }

    // logger_->Log("Average page size: " + std::to_string(avg_size) + " bytes");

    // ss.str(std::string());
    // ss << std::fixed << std::setprecision(2) << growth_ratio;
    // logger_->Log("Growth ratio: " + ss.str() + " links/page");
#endif
    mtx_.unlock();
  }
}

void Crawler::PrintInfoCallback() {
  static int counter = 0;
  while (!Terminate()) {
    instance_->print_mtx_.lock();
    PrintInfo(counter % 30 == 0); // Print the complete information every 10 ticks.
    instance_->print_mtx_.unlock();
    ++counter;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  }
}

void Crawler::SignalHandler(int signum) {
   instance_->print_mtx_.lock();
   instance_->logger_->Log("Interrupt signal (" + std::to_string(signum) + ") received.\n");

   // Prevent storage from writing anything else.
   instance_->terminate_ = true;

   instance_->storage_->StopWriting();

   // Commit cursor positions.
#if !defined IN_MEMORY
   instance_->url_priority_list_->FetchBlock();
#endif

   instance_->PrintInfo(true);
   instance_->print_mtx_.unlock();
   exit(signum);  
}

void Crawler::Start(
  const char* out_file, const char* db_file, const char* pl_file, bool overwrite
) {
  std::cout << "Started" << std::endl;
  instance_ = this;
  now_ = system_clock::now();
  bytes_written_ = 0;
  fetched_urls_num_ = 0;
  empty_db_file_ = false;
  total_time_ = 0;
  sample_time_ = system_clock::now();
  scheduler_->ClearDelayedQueue();

  storage_->Open(out_file, overwrite);

  // struct sigaction setup_action;
  // sigset_t block_mask;
  // sigaddset (&block_mask, SIGINT);
  // sigaddset (&block_mask, SIGQUIT);
  // pthread_sigmask(SIG_BLOCK, &block_mask, NULL);

  logger_->Log(db_file);
  logger_->Log(pl_file);
#ifndef IN_MEMORY
  url_database_->Open(db_file, overwrite);
#endif
#ifndef IN_MEMORY_PRIORITY_LIST
  url_priority_list_->Open(pl_file, overwrite),
#endif
 
  scheduler_->RegisterUrlAsync("noticias.terra.com.br");

  delayed_urls_thread_ = std::thread(&Crawler::ProcessDelayedUrls, this);
  print_info_thread_ = std::thread(&Crawler::PrintInfoCallback, this);
  for (int i = 0; i < num_threads_; ++i) {
    threads_[i] = std::thread(&Crawler::FetchPagesAsync, this, i);
  }

  for (int i = 0; i < num_threads_; ++i) {
    if (threads_[i].joinable()) threads_[i].join();
  }
  if (delayed_urls_thread_.joinable()) delayed_urls_thread_.join();
  if (print_info_thread_.joinable()) print_info_thread_.join();

  PrintInfo(true);
  storage_->Close();

#ifndef IN_MEMORY
  url_database_->Close();
#endif
#ifndef IN_MEMORY_PRIORITY_LIST
  url_priority_list_->Close();
#endif

  std::cout << "Finished." << std::endl;
}

void Crawler::TestNumberOfThreads() {
  max_urls_ = 1000;
  size_t speed[64];
  size_t file_size[64];
  for (int i = 8; i <= 512; i += 8) {
    logger_->Log("Testing " + std::to_string(i) + " threads.");
    num_threads_ = i;
    Start(
      "/mnt/hd0/joao_test/threads/html_pages_test",
      "/mnt/hd0/joao_test/threads/db_test",
      "/mnt/hd0/joao_test/threads/pl_test",
      true
    );
    system_clock::time_point now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now - now_); 
    speed[(i/8) - 1] = total_time_;
    file_size[(i/8) - 1] = file_size_;
    std::cout << "Ended test with speed " << total_time_ << "\n";
  }
  for (int i = 0; i < 64; ++i) {
    std::cout << (i + 1) * 8 << ", " << speed[i] << ", " << file_size[i] << "\n";
  }
  std::cout << "End\n";
}

} // End of namespace.
