#include "url_database.hpp"
#include <cassert>

using namespace std::chrono;

int main () {
  std::shared_ptr<Crawler::ILogger> logger;
  logger = std::make_shared<Crawler::Logger>();
  std::shared_ptr<Crawler::IUrlDatabase> db;
  db = std::make_shared<Crawler::UrlDatabase>(logger);

  system_clock::time_point timestamp = system_clock::time_point::min();

  db->set_table_size(1000);
  db->Open("build/db_test", true);
  assert(db->Put("lorem ipsum", timestamp));
  assert(db->Put("dolor sit", timestamp));

  Crawler::Entry entry;
  assert(db->Get("lorem ipsum", &entry));
  assert(db->Get("dolor sit", &entry));
  assert(!db->Get("amet consectetur", &entry));
  db->Close();
  return 0;
}
