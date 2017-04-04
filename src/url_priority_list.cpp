#include "url_priority_list.hpp"

namespace Crawler {

UncrawledUrl::UncrawledUrl(
  std::string url, system_clock::time_point scheduled_time
) : url(url), scheduled_time(scheduled_time) {
}

UrlPriorityList::UrlPriorityList(std::shared_ptr<ILogger> logger) 
  : logger_(logger), num_uncrawled_urls_(0), num_registered_urls_(0) {
}

UrlPriorityList::~UrlPriorityList() {
}

int UrlPriorityList::GetPriority(const std::string& url) {
  size_t pos = 0, words = 0;
  while (pos != std::string::npos) {
    pos = url.find_first_of('/', pos + 1);
    ++words;
  }
  return (words > PRIORITY_LEVELS) ? (PRIORITY_LEVELS - 1) : words - 1;
}

bool UrlPriorityList::Push(
  const std::string& url, const system_clock::time_point& scheduled_time
) {
  if (url.size() == 0 || url.size() > 256) return false;

  auto ms = duration_cast<milliseconds>(scheduled_time - system_clock::now());
  if (ms.count() > MAX_WAIT_TIME) {
    // If we have to wait too much to query this server again, probably it is
    // not worth it to add this url. 
    return false;
  }

  int priority = GetPriority(url);
  if (priority > MAX_PRIORITY_LEVEL) return false;

  if (priority != 0 && uncrawled_urls_[priority].size() > MAX_UNCRAWLED_URLS_STORED) 
    return false;

  uncrawled_urls_[priority].push(UncrawledUrl(url, scheduled_time));
  ++num_registered_urls_;
  ++num_uncrawled_urls_;
  return true;
}

bool UrlPriorityList::Pop(std::string* url) {
  for (int i = 0; i <= MAX_PRIORITY_LEVEL; ++i) {
    if (uncrawled_urls_[i].size() == 0) continue;

    system_clock::time_point scheduled_time;
    scheduled_time = uncrawled_urls_[i].top().scheduled_time;

    // If the first uncrawled url in the priority queue is still not due
    // then there are no available urls in this queue. 
    // We need to move to the next one.
    auto ms = duration_cast<milliseconds>(system_clock::now() - scheduled_time);
    if (ms.count() < 0) continue;

    *url = uncrawled_urls_[i].top().url;
    uncrawled_urls_[i].pop();
    --num_uncrawled_urls_;
    return true;
  }
  return false;
}

size_t UrlPriorityList::GetNumUrlsAtPriorityLevel(const int& i) {
  return uncrawled_urls_[i].size();
}

void UrlPriorityList::Clear() {
  for (int i = 0; i <= MAX_PRIORITY_LEVEL; ++i) {
    uncrawled_urls_[i] = PriorityQueue();
    num_registered_urls_ = 0;
    num_uncrawled_urls_ = 0;
  }
}

} // End of namespace.
