#include "injector.hpp"
#include <iostream>

int main () {
  Crawler::Injector::Instance()->crawler()->Start();
  return 0;
}
