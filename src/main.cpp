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
  if (argc == 1) {
    std::cout << "Output will be written to the current directory.\n";
  } else if (argc != 3 && argc != 4) {
    std::cout << "Usage: crawler OUTPUT_FILE DATABASE_FILE [MAX_URLS]\n";
  }
  std::cout << "IMPORTANT: the crawler may take up to 300 seconds to reach its full speed due to the politeness policy and the small number of seeds.\n";

  unsigned int max_urls = (argc == 4) ? std::atoi(argv[3]) : 0;
  std::string output_file = (argc > 3) ? argv[1] : "html_pages";
  std::string db_file = (argc > 3) ? argv[2] : "db_file";

#ifdef PROFILE
  ProfilerStart("crawler.prof");
#endif
  signal(SIGINT, Crawler::Crawler::SignalHandler);
  Crawler::Injector::Instance()->crawler()->Start(
    output_file.c_str(), db_file.c_str(), max_urls
  );
#ifdef PROFILE
  ProfilerStop();
#endif
  return 0;
}
