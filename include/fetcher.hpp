/**
 * Fetcher class
 * @author: jmfveneroso@gmail.com.br
 */

#ifndef __FETCHER_HPP__
#define __FETCHER_HPP__

#include <vector>
#include "logger.hpp"
#include "scheduler.hpp"
#include "CkSpider.h"

namespace Crawler {

enum FetcherState {
  IDLE = 0,
  FETCHING,
  FAILED
};

struct WebPage {
  bool failed;
  std::string html;
  std::vector<std::string> links;
};

class IFetcher {
 public:
  virtual ~IFetcher() {}
  virtual WebPage* GetWebPage(const std::string& url) = 0;
};

class Fetcher : public IFetcher {
  FetcherState state_ = IDLE;

 public:
  Fetcher();

  FetcherState state() { return state_; }
  void set_state(FetcherState state) { state_ = state; }
  WebPage* GetWebPage(const std::string& url);
};

} // End of namespace.

#endif
