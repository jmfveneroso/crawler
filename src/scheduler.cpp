#include "scheduler.hpp"

namespace Crawler {

Scheduler::Scheduler(
  std::shared_ptr<ILogger> logger,
  std::shared_ptr<IUrlDatabase> url_database,
  std::shared_ptr<IUrlPriorityList> url_priority_list
) : logger_(logger), url_database_(url_database), 
    url_priority_list_(url_priority_list) {
}

std::string Scheduler::TruncateUrl(std::string url) {
  if (url.find("http://")  == 0)  url = url.substr(7);
  if (url.find("https://") == 0)  url = url.substr(8);
  if (url.find("www.")     == 0)  url = url.substr(4);
  if (url[url.size() - 1] == '/') url = url.substr(0, url.size() - 1);
  return (url.size() <= 255) ? url : url.substr(0, 255);
}

std::string Scheduler::GetRootUrl(std::string url) {
  url = TruncateUrl(url);
  size_t pos = url.find_first_of('/');
  return url.substr(0, pos);
}

bool Scheduler::CheckRootValidity(const std::string& url) {
  if (privileged_roots_.find(url) != privileged_roots_.end()) {
    return true;
  }

  if (url.size() < 3) return false;
  if (url.substr(url.size() - 3) != ".br") {
    logger_->Log("Ignoring url: " + url, ONLY_ON_VERBOSE);
    return false;
  }
  return true;
}

bool Scheduler::RegisterUrl(const std::string& url) {
  if (url.size() == 0 || url.size() > 255) return false;

  std::string truncated_url = TruncateUrl(url);
  std::string root_url = GetRootUrl(url);
  if (!CheckRootValidity(root_url)) return false;

  if (url_priority_list_->GetPriority(truncated_url) > MAX_PRIORITY_LEVEL)
    return false;

  // Checks if url is unique.
  Entry entry;
  if (url_database_->Get(truncated_url, &entry)) {
    logger_->Log("Not unique: " + truncated_url, ONLY_ON_VERBOSE);
    return false;
  }

  // Writes unique url to database and find the scheduled time.
  system_clock::time_point scheduled_time = system_clock::now();
  try {
    if (truncated_url == root_url) {
      if (!url_priority_list_->Push(root_url, system_clock::time_point())) {
        return false;
      }
    } else {
      // Check if root url exists.
      bool register_root = false;
      if (url_database_->Get(root_url, &entry)) {
        scheduled_time = entry.timestamp + milliseconds(POLITENESS_POLICY); 
      } else {
        scheduled_time += milliseconds(POLITENESS_POLICY);
        register_root = true;
      }

      if (!url_priority_list_->Push(truncated_url, scheduled_time)) {
        // The queue is already full.
        return false;
      }
      if (register_root) RegisterUrl(root_url);
      url_database_->Put(truncated_url, scheduled_time);
    }
    // Record the last scheduled time for that root url.
    url_database_->Put(root_url, scheduled_time);
  } catch (std::runtime_error& e) {
    logger_->Log(e.what());
    return false;
  }

  logger_->Log(std::string("Added: ") + truncated_url + ".", ONLY_ON_VERBOSE);
  return true;
}

bool Scheduler::RegisterUrlAsync(const std::string& url) {
  mtx_.lock();
  bool result = RegisterUrl(url);
  mtx_.unlock();
  return result;
}

bool Scheduler::GetNextUrl(std::string* url) {
  mtx_.lock();

  // Check if there are any uncrawled urls waiting to be queried.
  if (!url_priority_list_->Pop(url)) {
    // The url queue is empty or there are no servers available 
    // due to the politeness policy. 
    mtx_.unlock();
    return false;
  }

  mtx_.unlock();
  return true;
}

void Scheduler::AddPrivilegedRoot(const std::string& url) {
  privileged_roots_.insert(url);
}

} // End of namespace.
