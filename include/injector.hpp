#ifndef __INJECTOR_HPP__
#define __INJECTOR_HPP__

#include <memory>
#include "CkSpider.h"
#include "logger.hpp"
#include "scheduler.hpp"
#include "storage.hpp"
#include "crawler.hpp"
#include "url_database.hpp"
#include "url_priority_list.hpp"

namespace Crawler {

class Injector {
 static Injector* instance_;
 std::shared_ptr<CkSpider> ck_spider_;
 std::shared_ptr<ILogger> logger_;
 std::shared_ptr<IUrlDatabase> url_database_;
 std::shared_ptr<IUrlPriorityList> url_priority_list_;
 std::shared_ptr<IScheduler> scheduler_;
 std::shared_ptr<IStorage> storage_;
 std::shared_ptr<ICrawler> crawler_;

 public:
  Injector();
  ~Injector();

  static Injector* Instance();
  std::shared_ptr<ICrawler> crawler();
  std::shared_ptr<IUrlDatabase> url_database();
  std::shared_ptr<IUrlPriorityList> url_priority_list();
  std::shared_ptr<IScheduler> scheduler();
};

} // End of namespace.

#endif
