#include <iostream>
#include <vector>
#include <string>
#include "CkSpider.h"
#include "CkString.h"

using namespace std;

int main(){
  CkSpider spider;
  string html, url;

  // No need to call Initialize here since Initialize restrains the crawler
  // to crawl only within the specified domain.
  // spider.Initialize("chilkatsoft.com");

  spider.AddUnspidered("dadwadawdawda");
  bool a = spider.CrawlNext(); // bool return
  if (!a) {
    std::cout << spider.getFailedUrl(0) << std::endl;
    return 1;
  }
  std::cout << a << std::endl;

  int size = spider.get_NumUnspidered();

  html = spider.lastHtml(); // Saves HTML

  cout << "Crawled HTML: " << html << '\n';

  // Inbound links
  for (int i = 0; i < size; i++) {
          url = spider.getUnspideredUrl(0);
          spider.SkipUnspidered(0); // Removes inbound link from local queue
          cout << "Crawled URL: " << url << '\n';
  }

  // Outbound links
  size = spider.get_NumOutboundLinks();
  for (int i = 0; i < size; i++) {
          url = spider.getOutboundLink(i);
          cout << "Crawled outbound URL: " << url << '\n';
  }

  spider.ClearOutboundLinks(); // Clears all outbound links
}
