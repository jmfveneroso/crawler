#ifndef __INJECTOR_HPP__
#define __INJECTOR_HPP__

#include <memory>
#include "CkSpider.h"
#include "logger.hpp"
#include "scheduler.hpp"
#include "fetcher.hpp"
#include "storage.hpp"
#include "crawler.hpp"

namespace Crawler {

class Injector {
 static Injector* instance_;
 std::shared_ptr<CkSpider> ck_spider_;
 std::shared_ptr<ILogger> logger_;
 std::shared_ptr<IScheduler> scheduler_;
 std::shared_ptr<IFetcher> fetcher_;
 std::shared_ptr<IStorage> storage_;
 std::shared_ptr<ICrawler> crawler_;

 public:
  Injector();
  ~Injector();

  static Injector* Instance();
  std::shared_ptr<ICrawler> crawler();
};

} // End of namespace.

#endif
