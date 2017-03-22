#include <iostream>
#include "injector.hpp"

int main () {
  std::shared_ptr<Crawler::ICrawler> c;
  c = Crawler::Injector::Instance()->crawler();
  c->Start();
  return 0;
}
