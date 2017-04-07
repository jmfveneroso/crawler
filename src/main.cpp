#include "injector.hpp"
#include <iostream>
#include <fstream>
#include <streambuf>
#include <csignal>

// Google profiler.
#ifdef PROFILE
#include <gperftools/profiler.h>
#endif

int main (int argc, char** argv) {
  if (argc != 3 && argc != 4) {
    std::cout << "Usage: crawler OUTPUT_FILE DATABASE_FILE [MAX_URLS]\n";
    return 1;
  }

  unsigned int max_urls = (argc == 4) ? std::atoi(argv[3]) : 0;

#ifdef PROFILE
  ProfilerStart("crawler.prof");
#endif
  signal(SIGINT, Crawler::Crawler::SignalHandler);
  Crawler::Injector::Instance()->crawler()->Start(
    argv[1], argv[2], max_urls
  );
#ifdef PROFILE
  ProfilerStop();
#endif
  return 0;
}
