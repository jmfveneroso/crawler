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

bool Scheduler::ProcessDelayedQueue() {
  mtx_.lock();
  if (delayed_queue_.empty()) {
    mtx_.unlock();
    return false;
  }
  // logger_->Log("delayed_queue: " + std::to_string(delayed_queue_.size()));
  // logger_->Log("ready_queue: " + std::to_string(ready_queue_.size()));

  // Only the first inserted element is checked. Some elements may have to
  // wait more than the amount of time specified by the politeness policy
  // but never less.
  DelayedUrl delayed_url = delayed_queue_.front();
  system_clock::time_point now = system_clock::now();
  auto ms = duration_cast<milliseconds>(now - delayed_url.timestamp);
  if (ms.count() < politeness_policy_) {
    mtx_.unlock();
    return false;
  }

  ready_queue_.push(delayed_url.url);
  delayed_queue_.pop();
  mtx_.unlock();
  return true;
  // mtx_.unlock();
  // return false;
}

bool Scheduler::RegisterUrl(const std::string& url) {
  if (url.size() == 0 || url.size() > 255) return false;

  std::string truncated_url = TruncateUrl(url);
  std::string root_url = GetRootUrl(url);
  if (root_url.size() < 3) return false;
  if (root_url.substr(root_url.size() - 3) != ".br") {
#ifdef VERBOSE
    logger_->Log("Ignoring url: " + url);
#endif
    return false;
  }

  // Checks if url is unique.
  Entry entry;
  if (url_database_->Get(truncated_url, &entry)) {
#ifdef VERBOSE
    logger_->Log("not unique: " + truncated_url);
#endif
    return false;
  }

  // Writes unique url to database.
  try {
    if (url_database_->Put(truncated_url, system_clock::time_point())) {
#ifdef VERBOSE
      logger_->Log(std::string("Added: ") + truncated_url + ".");
#endif
      // Pushes url to queue.
      if (url_priority_list_->Push(truncated_url)) {
        // return false;
      }
    } else {
      return false;
    }
  } catch (std::runtime_error& e) {
    logger_->Log(e.what());
  }
  return true;
}

bool Scheduler::RegisterUrlAsync(const std::string& url) {
  mtx_.lock();
  bool result = RegisterUrl(url);
  mtx_.unlock();
  return result;
}

bool Scheduler::EnforcePolitenessPolicy(
  const std::string& url, bool write_to_priority_list
) {
  std::string root_url = GetRootUrl(url);

  system_clock::time_point now = system_clock::now();

  Entry entry;
  if (!url_database_->Get(root_url, &entry)) {
    // The root url has not been added to the queue yet.
    RegisterUrl(root_url);
  } else {
    auto ms = duration_cast<milliseconds>(now - entry.timestamp);
    if (ms.count() < politeness_policy_) {
      // The root server has already been accessed recently.
      // We must delay the request for this url.
      if (write_to_priority_list) {
        url_priority_list_->Push(url, false);
      } else {
        delayed_queue_.push(DelayedUrl(url, entry.timestamp));
      }

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
  bool send_back_to_disk = false;

  // Check if there are any delayed urls waiting to be queried.
  if (!ready_queue_.empty()) {
    *url = ready_queue_.front();
    ready_queue_.pop();
    send_back_to_disk = true;
  } else if (!url_priority_list_->Pop(url)) {
    // If the url queue is empty, we must fetch the next batch from disk.
    // logger_->Log("Empty queue.");
    mtx_.unlock();
    return false;
  }

  // Fetch a url that abides to the politeness policy, while adding the
  // ones that do not abide to the delayed queue.
  while (!EnforcePolitenessPolicy(*url, send_back_to_disk)) {
    // logger_->Log("Not polite now: " + *url);
    if (!ready_queue_.empty()) {
      *url = ready_queue_.front();
      ready_queue_.pop();
      send_back_to_disk = true;
      continue;
    }
    // We must wait to query this url. Let's try another one.
    else if (!url_priority_list_->Pop(url)) {
      mtx_.unlock(); 
      return false;
    }
    send_back_to_disk = false;
  }

  mtx_.unlock();
  return true;
}

void Scheduler::ClearDelayedQueue() {
  mtx_.lock();
  // delayed_queue_ = PriorityQueue();
  delayed_queue_ = std::queue<DelayedUrl>();
  ready_queue_ = std::queue<std::string>();
  mtx_.unlock();
}

} // End of namespace.
