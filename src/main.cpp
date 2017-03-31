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
    "/mnt/hd0/joao_test/memo_secondary/html_pages",
    "/mnt/hd0/joao_test/memo_secondary/db",
    "/mnt/hd0/joao_test/memo_secondary/pl_"
  );
  // Crawler::Injector::Instance()->crawler()->TestNumberOfThreads();
#ifdef PROFILE
  ProfilerStop();
#endif
  return 0;
}
