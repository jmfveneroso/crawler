#include "fetcher.hpp"

namespace Crawler {

Fetcher::Fetcher(
  std::shared_ptr<ILogger> logger,
  std::shared_ptr<CkSpider> ck_spider
) : logger_(logger),
    ck_spider_(ck_spider) {
}

WebPage Fetcher::GetWebPage(const std::string& url) {
  CkSpider spider;

  // No need to call Initialize here since Initialize restrains the crawler
  // to crawl only within the specified domain.
  // spider.Initialize("chilkatsoft.com");

  WebPage web_page;

  spider.AddUnspidered(url.c_str());
  if (!spider.CrawlNext()) {
    spider.ClearFailedUrls();
    return web_page;
  }

  web_page.html = spider.lastHtml();

  // Inbound links.
  for (int i = 0; i < spider.get_NumUnspidered(); ++i) {
    std::string link = spider.getUnspideredUrl(0);
    spider.SkipUnspidered(0); // Removes inbound link from chilkat queue.
    web_page.inbound_links.push_back(link);
  }

  // Outbound links.
  for (int i = 0; i < spider.get_NumOutboundLinks(); ++i) {
    std::string link = spider.getOutboundLink(i);
    web_page.outbound_links.push_back(link);
  }

  spider.ClearOutboundLinks();
  spider.ClearSpideredUrls();
  return web_page;
}

} // End of namespace.
