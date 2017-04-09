#include "url_priority_list.hpp"
#include <iostream>
#include <cassert>

using namespace std::chrono;

int main () {
  std::shared_ptr<Crawler::ILogger> logger;
  logger = std::make_shared<Crawler::Logger>();
  std::shared_ptr<Crawler::IUrlPriorityList> pl;
  pl = std::make_shared<Crawler::UrlPriorityList>(logger);

  system_clock::time_point time;
  assert(pl->Push("www.aaa.com", time));
  assert(pl->Push("www.aaa.com/bbb", time));
  assert(pl->Push("ddd.com/1/2/3/4/5/6/7/8/9/10/11/12", time));
  assert(pl->Push("ccc.com", time));
  time = system_clock::now() + milliseconds(10000); 
  assert(pl->Push("ddd.com/eee/fff/ggg", time));

  std::string url;

  assert(pl->Pop(&url));
  assert(url == "www.aaa.com");
  assert(pl->Pop(&url));
  assert(url == "ccc.com");
  assert(pl->Pop(&url));
  assert(url == "www.aaa.com/bbb");
  assert(pl->Pop(&url));
  assert(url == "ddd.com/1/2/3/4/5/6/7/8/9/10/11/12");
  assert(!pl->Pop(&url));
  // assert(url == "ddd.com/eee/fff/ggg");

  return 0;
}
