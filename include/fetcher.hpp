/**
 * Fetcher class
 * @author: jmfveneroso@gmail.com.br
 */

#ifndef __FETCHER_HPP__
#define __FETCHER_HPP__

#include <string>
#include <vector>
#include "logger.hpp"
#include "CkSpider.h"

namespace Crawler {

struct WebPage {
  bool failed;
  std::string html;
  std::vector<std::string> links;
};

class IFetcher {
 public:
  virtual ~IFetcher() {}
  virtual void GetWebPage(const std::string& url, WebPage* web_page) = 0;
};

class Fetcher : public IFetcher {
 public:
  Fetcher();

  void GetWebPage(const std::string& url, WebPage* web_page);
};

} // End of namespace.

#endif
