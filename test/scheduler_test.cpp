#include "injector.hpp"
#include <iostream>
#include <cassert>

using namespace std::chrono;

int main() {
  Crawler::Injector *inj = Crawler::Injector::Instance();

  std::shared_ptr<Crawler::IUrlDatabase> db = inj->url_database();
  db->set_table_size(1000);
  db->Open("build/db_test", true);

  std::shared_ptr<Crawler::IScheduler> s = Crawler::Injector::Instance()->scheduler();
  assert(!s->RegisterUrlAsync("www.aaa.com"));
  assert(s->RegisterUrlAsync("www.aaa.com.br"));
  assert(!s->RegisterUrlAsync("www.aaa.com.br"));
  assert(s->RegisterUrlAsync("www.aaa.com.br/bbb"));

  std::string url; 
  assert(s->GetNextUrl(&url));
  assert("aaa.com.br" == url);
  assert(!s->GetNextUrl(&url));

  db->Close();

  return 0;
}
