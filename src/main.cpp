#include "injector.hpp"
#include <iostream>
#include <fstream>
#include <streambuf>
#include <csignal>

// Google profiler.
#ifdef PROFILE
#include <gperftools/profiler.h>
#endif

int main () {
  signal(SIGINT, Crawler::Crawler::SignalHandler);
#ifdef PROFILE
  ProfilerStart("crawler.prof");
#endif
  Crawler::Injector::Instance()->crawler()->Start(
    "html_pages",
    "db",
    "pl_"
  );
#ifdef PROFILE
  ProfilerStop();
#endif
  return 0;
}
