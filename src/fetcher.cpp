#include "fetcher.hpp"
#include <iostream>

namespace Crawler {

Fetcher::Fetcher() {
  // spider_.put_ConnectTimeout(5);
  // spider_.put_MaxResponseSize(250000); // 250kb.
}

void Fetcher::GetWebPage(const std::string& url, WebPage* web_page) {
  CkSpider spider;

  // No need to call Initialize here since Initialize restrains the crawler
  // to crawl only within the specified domain.
  // spider.Initialize(url.c_str());

  web_page->failed = false;

  spider.AddUnspidered(url.c_str());

  if (!spider.CrawlNext()) {
    web_page->failed = true;
    state_ = FAILED;
    return;
  }

  web_page->html = spider.lastHtml();

  // Inbound links.
  for (int i = 0; i < spider.get_NumUnspidered(); ++i) {
    std::string link = spider.getUnspideredUrl(0);
    spider.SkipUnspidered(0); // Removes inbound link from chilkat queue.
    web_page->links.push_back(link);
  }

  // Outbound links.
  for (int i = 0; i < spider.get_NumOutboundLinks(); ++i) {
    std::string link = spider.getOutboundLink(i);
    web_page->links.push_back(link);
  }
}

} // End of namespace.
