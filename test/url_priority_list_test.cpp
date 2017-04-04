#include "url_priority_list.hpp"
#include <iostream>
#include <cassert>

int main () {
  // std::shared_ptr<Crawler::ILogger> logger;
  // logger = std::make_shared<Crawler::Logger>();
  // std::shared_ptr<Crawler::IUrlPriorityList> pl;
  // pl = std::make_shared<Crawler::UrlPriorityList>(logger);

  // pl->Open("build/priority_file_", true);
  // assert(pl->Push("www.aaa.com"));
  // assert(pl->Push("www.aaa.com/bbb"));
  // assert(pl->Push("ddd.com/1/2/3/4/5/6/7/8/9/10/11/12"));
  // assert(pl->Push("ccc.com"));
  // assert(pl->Push("ddd.com/eee/fff/ggg"));

  // std::string url;
  // assert(!pl->Pop(&url));
 
  // assert(pl->FetchBlock() == 5);
  // pl->Close();

  // pl->Open("build/priority_file_");
  // assert(pl->FetchBlock() == 5);

  // assert(pl->Pop(&url));
  // assert(url == "www.aaa.com");
  // assert(pl->Pop(&url));
  // assert(url == "ccc.com");
  // assert(pl->Pop(&url));
  // assert(url == "www.aaa.com/bbb");
  // assert(pl->Pop(&url));
  // assert(url == "ddd.com/eee/fff/ggg");
  // assert(pl->Pop(&url));
  // assert(url == "ddd.com/1/2/3/4/5/6/7/8/9/10/11/12");
  // pl->Close();

  return 0;
}
