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
  if (argc != 3) {
    std::cout << "Usage: thread_test OUTPUT_FILE DATABASE_FILE\n";
    return 1;
  }

#ifdef PROFILE
  ProfilerStart("crawler.prof");
#endif
  signal(SIGINT, Crawler::Crawler::SignalHandler);
  Crawler::Injector::Instance()->crawler()->TestNumberOfThreads(
    argv[1], argv[2]
  );
#ifdef PROFILE
  ProfilerStop();
#endif
  return 0;
}
