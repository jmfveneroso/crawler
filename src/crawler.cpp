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

void Crawler::BlockSignals() {
  // Block terminal-generated signals at the fetcher threads.
  // Only the main thread should be responsible for intercepting signals.
  struct sigaction setup_action;
  sigset_t block_mask;
  sigemptyset (&block_mask);
  sigaddset (&block_mask, SIGINT);
  sigaddset (&block_mask, SIGQUIT);
  setup_action.sa_mask = block_mask;
  setup_action.sa_flags = 0;
  sigaction (SIGTSTP, &setup_action, NULL);
}

void Crawler::FetchPagesAsync(int fetcher_pos) {
  BlockSignals();
  while (!Terminate()) {
    mtx_.lock();

    // Fetch the next uncrawled url.
    std::string url;
    if (!scheduler_->GetNextUrl(&url)) {
      // If there are no urls available, wait until they are due or
      // more roots are scheduled.
      mtx_.unlock();
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      continue;
    }
    mtx_.unlock();

    logger_->Log(std::string("Started fetching ") + url, ONLY_ON_VERBOSE);

    // Here is the only relevant place where the async magic happens.
    WebPage web_page;
    fetchers_[fetcher_pos].GetWebPage(url, &web_page); 

    mtx_.lock();
    if (web_page.failed || web_page.html.size() == 0) {
      logger_->Log(std::string("Failed url: ") + url, ONLY_ON_VERBOSE);
      ++failed_urls_num_;
      mtx_.unlock();
      continue;
    }

    for (auto link : web_page.links) {
      if (!scheduler_->RegisterUrlAsync(link)) continue;
      if (max_urls_ != 0) ++total_urls_;
    }
    mtx_.unlock();

    // Storage has its own mutex.
    size_t bytes = storage_->Write(url, web_page.html);
    bytes_written_ += bytes;
    if (bytes == 0) {
      logger_->Log("Url " + url + " was not written to disk.", ONLY_ON_VERBOSE);      
    } else {
      if (max_urls_ != 0) ++pages_written_;
      ++fetched_urls_num_;
      ++sample_size_;
      logger_->Log("Fetched url " + url, ONLY_ON_VERBOSE);
    }
  }

  // Sometimes a few threads will get stuck waiting for responses when
  // the max number of urls has already been downloaded. In this case
  // the first one that ends willl calculate here the amount of time the program took.
  if (total_time_ == 0) {
    system_clock::time_point now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now - start_time_); 
    total_time_ = ms.count();
    mtx_.lock();
    storage_->StopWriting();
    mtx_.unlock();
  }
}

void Crawler::PrintInfo() {
  system_clock::time_point now = system_clock::now();
  auto ms = duration_cast<milliseconds>(now - start_time_);
  size_t num_registered_urls = url_priority_list_->num_registered_urls();

  logger_->Log("==================================================");
  logger_->Log("Elapsed time (ms): " + std::to_string(ms.count())          + "");
  logger_->Log("Webpages fetched: "  + std::to_string(fetched_urls_num_)   + "");
  logger_->Log("Failed urls: "       + std::to_string(failed_urls_num_)    + "");
  logger_->Log("Registered urls: "   + std::to_string(num_registered_urls) + "");

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

#ifndef IN_MEMORY
  double percentage = (double) url_database_->num_unique_urls() / TABLE_SIZE;
  ss.str(std::string());
  ss << std::fixed << std::setprecision(2) << 100 - percentage * 100;
  logger_->Log("Hash table (empty): " + ss.str() + "%");
#endif

  total_urls_ = 0;
  for (int i = 0; i <= MAX_PRIORITY_LEVEL; ++i) {
    size_t num_urls = url_priority_list_->GetNumUrlsAtPriorityLevel(i);
    logger_->Log("URLs at priority level " + std::to_string(i) + ": " + std::to_string(num_urls) + "");
    total_urls_ += num_urls;
  }
  logger_->Log("URLs total: " + std::to_string(total_urls_) + "");

  logger_->Log("File size: " + std::to_string(bytes_written_)    + "");
  size_t avg_size = 0;
  double growth_ratio = 0;
  if (fetched_urls_num_ > 0) {
    avg_size = (bytes_written_ / fetched_urls_num_) - 4;
    growth_ratio = ((double) num_registered_urls / fetched_urls_num_);
  }

  logger_->Log("Average page size: " + std::to_string(avg_size) + " bytes");

  ss.str(std::string());
  ss << std::fixed << std::setprecision(2) << growth_ratio;
  logger_->Log("Growth ratio: " + ss.str() + " links/page");
}

void Crawler::PrintInfoCallback() {
  while (!Terminate()) {
    instance_->print_mtx_.lock();
    PrintInfo(); 
    instance_->print_mtx_.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
  }
}

void Crawler::SignalHandler(int signum) {
   instance_->print_mtx_.lock();
   instance_->logger_->Log("Interrupt signal (" + std::to_string(signum) + ") received.\n");
   instance_->logger_->Log("Finishing write operation and cleaning up...\n");

   // Prevent storage from writing anything else.
   instance_->terminate_ = true;
   instance_->storage_->StopWriting();
   instance_->PrintInfo();
   instance_->print_mtx_.unlock();
   exit(signum);  
}

void Crawler::Seed() {
  // Won't be checking for ".br" in this root.
  scheduler_->AddPrivilegedRoot("g1.globo.com");

  // We gotta start from somewhere...
  scheduler_->RegisterUrlAsync("techtudo.com.br");
  scheduler_->RegisterUrlAsync("tecmundo.com.br");
  scheduler_->RegisterUrlAsync("info.abril.com.br");
  scheduler_->RegisterUrlAsync("g1.globo.com/tecnologia");
  scheduler_->RegisterUrlAsync("gizmodo.com.br");
  scheduler_->RegisterUrlAsync("uol.com.br");
}

void Crawler::Start(
  const char* out_file, const char* db_file, size_t max_urls
) {
  // This class needs to be a singleton because of the signal handling function.
  instance_ = this;

  // The output file always begins with "|||" which has a size of 3 chars.
  bytes_written_ = 3;
  fetched_urls_num_ = 0;
  total_time_ = 0;
  start_time_ = system_clock::now();
  sample_time_ = system_clock::now();
  max_urls_ = max_urls;

  // The output file will be overwritten.
  storage_->Open(out_file, true);

  std::cout << "Started!" << std::endl;
  logger_->Log("The output is being written to: " + std::string(out_file));

  // The url database may be built on disk or memory.
#ifndef IN_MEMORY
  logger_->Log("The database is being written to: " + std::string(db_file));
  url_database_->Open(db_file, true);
#else
  url_database_->Clear();
  logger_->Log("The database file " + std::string(db_file) + " will be ignored.");
#endif

  url_priority_list_->Clear();
  Seed();

  print_info_thread_ = std::thread(&Crawler::PrintInfoCallback, this);
  for (int i = 0; i < num_threads_; ++i) {
    threads_[i] = std::thread(&Crawler::FetchPagesAsync, this, i);
  }

  for (int i = 0; i < num_threads_; ++i) {
    if (threads_[i].joinable()) threads_[i].join();
  }
  if (print_info_thread_.joinable()) print_info_thread_.join();

  PrintInfo();
  storage_->Close();

#ifndef IN_MEMORY
  url_database_->Close();
#endif

  std::cout << "Finished!" << std::endl;
}

void Crawler::TestNumberOfThreads(const char* out_name, const char* db_name) {
  size_t speed[64];
  size_t file_size[64];
  for (int i = 8; i <= 512; i += 8) {
    logger_->Log("Testing " + std::to_string(i) + " threads.");
    num_threads_ = i;
    Start(
      out_name,
      db_name,
      1000
    );
    speed[(i/8) - 1] = total_time_;
    file_size[(i/8) - 1] = file_size_;
    std::cout << "Ended test with speed " << total_time_ << "\n";
  }

  // Print the output in a more easily parseable format.
  for (int i = 0; i < 64; ++i) {
    std::cout << (i + 1) * 8 << ", " << speed[i] << ", " << file_size[i] << "\n";
  }
  std::cout << "End\n";
}

} // End of namespace.
