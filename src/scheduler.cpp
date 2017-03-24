#include "scheduler.hpp"
#include <iostream>

namespace Crawler {

bool UrlComparator::GetUrlWordSize(std::string s) {
  size_t pos = 0, size = 0;
  while (pos != std::string::npos) {
    pos = s.find_first_of('/', pos + 1);
    ++size;
  }
  return size;
}

bool UrlComparator::operator() (std::string lft, std::string rgt) {
  return GetUrlWordSize(lft) < GetUrlWordSize(rgt);
}

Scheduler::Scheduler(std::shared_ptr<ILogger> logger) 
  : logger_(logger) {
  for (int i = 0; i < NUM_PARTITIONS; ++i) {
    partition_offset_[i] = 0;
  }
  Init();
}

bool Scheduler::ShouldWait(const std::string& url) {
  std::string root_url = GetRootUrl(url);
  if (last_requests_.find(root_url) != last_requests_.end()) {
    std::chrono::system_clock::time_point last_request = last_requests_[root_url];
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    auto difference = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_request);
    return difference.count() < 5000;
  }
  return false;
}

void Scheduler::WaitPolitelyAsync() {
  while (true) {
    if (waiting_queue_.empty()) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      continue;
    }

    if (ShouldWait(waiting_queue_.front())) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      logger_->Log("Wait more for: " + waiting_queue_.front());
      continue;
    }

    mtx_.lock();
    logger_->Log("Release the beast: " + waiting_queue_.front());
    std::string url = waiting_queue_.front();
    waiting_queue_.pop();
    url_queue_.push(url);
    mtx_.unlock();
  }
}

void Scheduler::Init() {
  queue_file_ = fopen("queue", "wb+");
  url_pos_ = 0;
  fwrite(&url_pos_, sizeof(size_t), 1, queue_file_);

  file_ = fopen("unique", "wb+");
  size_t size = NUM_PARTITIONS * sizeof(size_t);
  size += (size_t) NUM_PARTITIONS * PARTITION_SIZE * sizeof(size_t);

  for (size_t i = 0; i < size; ++i) { 
    char c = '\0';
    fwrite(&c, sizeof(char), 1, file_);
  }

  polite_thread_ = std::thread(&Scheduler::WaitPolitelyAsync, this);
  // polite_thread_.join();
}

size_t Scheduler::GetHash(std::string const& s) const {
  size_t result = 2166136261U;
  std::string::const_iterator end = s.end();
  for (
    std::string::const_iterator iter = s.begin();
    iter != end;
    ++iter
  ) {
    result = (16777619 * result) ^ static_cast<unsigned char>(*iter);
  }
  return result;
}

std::string Scheduler::GetRootUrl(std::string url) {
  if (url.find("http://")  == 0) url = url.substr(7);
  if (url.find("https://") == 0) url = url.substr(8);
  if (url.find("www.")     == 0) url = url.substr(4);

  size_t pos = url.find_first_of('/');
  return url.substr(0, pos);
}

long int Scheduler::GetPartitionOffset(size_t hash) {
  size_t partition_num = NUM_PARTITIONS * ((long double) hash / (size_t) - 1);
  long int offset = NUM_PARTITIONS * sizeof(size_t);
  offset += partition_num * PARTITION_SIZE * sizeof(size_t);
  return offset;
}

bool Scheduler::IsUnique(const std::string& url) {
  mtx_.lock();
  size_t hash = GetHash(url); 
  long int offset = GetPartitionOffset(hash);

  if (fseek(file_, offset, 0) == 0) {
    size_t buffer[PARTITION_SIZE];
    size_t count = fread(&buffer, sizeof(size_t), PARTITION_SIZE, file_);
    if (count != PARTITION_SIZE) {
      logger_->Log("Error reading partition.");
      mtx_.unlock();
      return false;
    }

    for (int i = 0; i < PARTITION_SIZE; ++i) {
      if (hash == buffer[i]) {
        mtx_.unlock();
        return false;
      }
    }
  } else {
    logger_->Log("ERRO 3");
  }
  mtx_.unlock();
  return true;
}

void Scheduler::SetUnique(const std::string& url) {
  mtx_.lock();
  size_t hash = GetHash(url); 
  long int offset = GetPartitionOffset(hash);

  size_t partition_num = NUM_PARTITIONS * ((long double) hash / (size_t) - 1);
  offset += partition_offset_[partition_num] * sizeof(size_t);

  if (partition_offset_[partition_num] >= PARTITION_SIZE) {
    logger_->Log("ERRO 1");
    mtx_.unlock();
    return;
  }

  if (fseek(file_, offset, 0) != 0) {
    logger_->Log("ERRO 2");
    mtx_.unlock();
    return;
  }

  fwrite(&hash, sizeof(size_t), 1, file_);
  partition_offset_[partition_num]++;
  mtx_.unlock();
}

bool Scheduler::AddUrl(const std::string& url) {
  // Check if url is unique.
  // if (crawled_urls_.find(url) != crawled_urls_.end()) return false;
  if (!IsUnique(url)) {
    logger_->Log("not unique: " + url);
    return false;
  }

  if (fseek(file_, 0, SEEK_END) == 0) {
    char const *c = url.c_str();
    fwrite(c, sizeof(char), url.size(), queue_file_);
    char nl = '\n';
    fwrite(&nl, sizeof(char), 1, queue_file_);
  }

  // url_queue_.push(url);

  SetUnique(url);

  // crawled_urls_.insert(url);

  logger_->Log(std::string("Added: ") + url + ".");
  return true;
}

bool Scheduler::ReadNextBatch() {
  if (fseeko(queue_file_, 0, SEEK_END) == 0) {
    off_t size = ftello(queue_file_);
    if (url_pos_ >= size) return false;
  }

  if (fseeko(queue_file_, 0, SEEK_SET) != 0) {
    fwrite(&url_pos_, sizeof(size_t), 1, queue_file_);
  }

  if (fseeko(queue_file_, url_pos_ + sizeof(off_t), SEEK_SET) != 0) {
    return false;
  }

  int counter = 0;
  char buffer[1000];
  while (fgets(buffer, 1000, queue_file_) != NULL) {
    std::string url(buffer);
    logger_->Log(url);
    url_pos_ += url.size();
    url_queue_.push(url);
    if (counter++ > 100) break;
  }
  return true;
}

std::string Scheduler::GetNextUrl() {
  if (url_queue_.empty()) {
    mtx_.lock();
    ReadNextBatch();
    mtx_.unlock();
    return "";
  }

  std::string url = url_queue_.top();

  // Check if url abides to politeness rule.
  std::string root_url = GetRootUrl(url);
  if (last_requests_.find(root_url) != last_requests_.end()) {
    std::chrono::system_clock::time_point last_request = last_requests_[root_url];
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    auto difference = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_request);
 
    std::cout << difference.count() << std::endl;
    if (difference.count() < 5000) {
      logger_->Log(std::string("Not polite ") + root_url);
      waiting_queue_.push(url);
      url_queue_.pop();
      return "";
    }
  }

  url_queue_.pop();
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  last_requests_.insert({ GetRootUrl(url), now });

  return url;
}

} // End of namespace.
