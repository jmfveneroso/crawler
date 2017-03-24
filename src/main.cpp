#include "injector.hpp"
#include <iostream>

int main () {
  std::shared_ptr<Crawler::IUrlDatabase> db;
  db = Crawler::Injector::Instance()->url_database();
  db->Open("correnegada");
  // db->Put("aba", 1);
  // db->Put("bab", 2);
  // db->Put("cac", 3);
  // db->Put("dcd", 4);
  // db->Put("efd", 5);
  // db->Put("fgf", 6);
  // db->Put("gag", 7);
  // db->Put("hbh", 8);
  // db->Put("iui", 9);
  // db->Put("j", 10);
  // db->Put("aba", 10);
  // db->Put("dawd", 10);
  // db->Put("k");

  Crawler::Entry e;
  db->Get("www.eminha.com", &e);

  while (1) {
    std::string x;
    std::cin >> x;
    if (db->Get(x, &e)) {
      std::cout << "Key " << x  << " found." << std::endl;
    } else {
      std::cout << "Key " << x  << " not found." << std::endl;
    }
  }
  // Crawler::Injector::Instance()->crawler()->Start();
  return 0;
}
