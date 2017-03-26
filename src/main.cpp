#include "injector.hpp"
#include <iostream>
#include <gperftools/profiler.h>
#include <fstream>
#include <streambuf>

int main () {
  ProfilerStart("crawler.prof");
  Crawler::Injector::Instance()->crawler()->Start();
  ProfilerStop();
  return 0;
}
