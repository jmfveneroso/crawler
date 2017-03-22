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

struct WebPage {
  std::string html;
  std::vector<std::string> inbound_links;
  std::vector<std::string> outbound_links;
};

class IFetcher {
 public:
  virtual ~IFetcher() {}
  virtual WebPage GetWebPage(const std::string& url) = 0;
};

class Fetcher : public IFetcher {
  std::shared_ptr<ILogger> logger_;
  std::shared_ptr<CkSpider> ck_spider_;

 public:
  Fetcher(
    std::shared_ptr<ILogger>, 
    std::shared_ptr<CkSpider>
  );

  WebPage GetWebPage(const std::string& url);
};

} // End of namespace.

#endif
