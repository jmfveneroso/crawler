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
  return (url.size() <= 256) ? url : url.substr(0, 256);
}

std::string Scheduler::GetRootUrl(std::string url) {
  url = TruncateUrl(url);
  size_t pos = url.find_first_of('/');
  return url.substr(0, pos);
}

bool Scheduler::ProcessDelayedQueue() {
  if (delayed_queue_.empty()) {
    return false;
  }
  // logger_->Log("delayed_queue: " + std::to_string(delayed_queue_.size()));

  DelayedUrl delayed_url = delayed_queue_.top();
  system_clock::time_point now = system_clock::now();
  auto ms = duration_cast<milliseconds>(now - delayed_url.timestamp);
  if (ms.count() < politeness_policy_) {
    return false;
  }

  mtx_.lock();
  delayed_queue_.pop();
  ready_queue_.push(delayed_url.url);
  mtx_.unlock();
  return true;
}

bool Scheduler::RegisterUrl(const std::string& url) {
  std::string truncated_url = TruncateUrl(url);

  // Checks if url is unique.
  Entry entry;
  if (url_database_->Get(truncated_url, &entry)) {
    // logger_->Log("not unique: " + truncated_url);
    return false;
  }

  // Writes unique url to database.
  try {
    url_database_->Put(truncated_url, system_clock::time_point());
  } catch (std::runtime_error& e) {
    logger_->Log(e.what());
  }
  logger_->Log(std::string("Added: ") + truncated_url + ".");

  // Pushes url to queue.
  url_priority_list_->Push(truncated_url);

  return true;
}

bool Scheduler::RegisterUrlAsync(const std::string& url) {
  mtx_.lock();
  bool result = RegisterUrl(url);
  mtx_.unlock();
  return result;
}

bool Scheduler::EnforcePolitenessPolicy(const std::string& url) {
  std::string root_url = GetRootUrl(url);

  system_clock::time_point now = system_clock::now();

  Entry entry;
  if (!url_database_->Get(root_url, &entry)) {
    // The root url has not been added to the queue yet.
    RegisterUrl(root_url);
  } else {
    auto ms = duration_cast<milliseconds>(now - entry.timestamp);
    if (ms.count() < politeness_policy_) {
      // logger_->Log("URL " + url + " está com problema de " + std::to_string(ms.count()));
      // The root server has already been accessed recently.
      // We must delay the request for this url.
      // logger_->Log(std::string("Not polite ") + url);
      delayed_queue_.push(DelayedUrl(url, entry.timestamp));
      return false;
    }
  }

  // Update the root url timestamp. So we will know if we must delay
  // other url requests.
  url_database_->Put(root_url, now);
  return true;
}

bool Scheduler::GetNextUrl(std::string* url) {
  mtx_.lock();
  // Check if there are any delayed urls waiting to be queried.
  if (!ready_queue_.empty()) {
    *url = ready_queue_.front();
    ready_queue_.pop();
    mtx_.unlock();
  } else if (!url_priority_list_->Pop(url)) {
    // If the url queue is empty, we must fetch the next batch from disk.
    // logger_->Log("Empty queue.");
    mtx_.unlock();
    return false;
  }

  // Fetch a url that abides to the politeness policy, while adding the
  // ones that do not abide to the delayed queue.
  while (!EnforcePolitenessPolicy(*url)) {
    // logger_->Log("Not polite now: " + *url);
    // We must wait to query this url. Let's try another one.
    if (!url_priority_list_->Pop(url)) {
      mtx_.unlock(); 
      return false;
    }
  }

  mtx_.unlock();
  return true;
}

} // End of namespace.
