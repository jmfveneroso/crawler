#include "injector.hpp"
#include <iostream>
#include <cassert>

using namespace std::chrono;

int main() {
  // Crawler::Injector *inj = Crawler::Injector::Instance();

  // std::shared_ptr<Crawler::IUrlDatabase> db = inj->url_database();
  // db->Open("build/db_test", true);

  // std::shared_ptr<Crawler::IUrlPriorityList> pl = inj->url_priority_list();
  // pl->Open("build/priority_file_", true);

  // std::shared_ptr<Crawler::IScheduler> s = Crawler::Injector::Instance()->scheduler();
  // s->set_politeness_policy(200); // Milliseconds.
  // assert(s->RegisterUrlAsync("www.aaa.com"));
  // assert(!s->RegisterUrlAsync("www.aaa.com"));
  // assert(s->RegisterUrlAsync("www.bbb.com"));
  // assert(s->RegisterUrlAsync("www.aaa.com/x"));

  // std::string url; 
  // assert(!s->GetNextUrl(&url));

  // assert(pl->FetchBlock() == 3);

  // assert(s->GetNextUrl(&url));
  // assert("aaa.com" == url);

  // assert(s->GetNextUrl(&url));
  // assert("bbb.com" == url);

  // assert(!s->GetNextUrl(&url));
  // s->ProcessDelayedQueue();
  // assert(!s->GetNextUrl(&url));

  // std::this_thread::sleep_for(std::chrono::milliseconds(100));
  // s->ProcessDelayedQueue();
  // assert(!s->GetNextUrl(&url));

  // std::this_thread::sleep_for(std::chrono::milliseconds(100));
  // s->ProcessDelayedQueue();
  // assert(s->GetNextUrl(&url));
  // assert("aaa.com/x" == url); 

  // db->Close();
  // pl->Close();

  return 0;
}
