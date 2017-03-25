#include "injector.hpp"

namespace Crawler {

Injector* Injector::instance_ = 0;

Injector::Injector() 
  : ck_spider_(std::make_shared<CkSpider>()),
    logger_(std::make_shared<Logger>()),
    url_database_(std::make_shared<UrlDatabase>(logger_)),
    url_priority_list_(std::make_shared<UrlPriorityList>(logger_)),
    scheduler_(std::make_shared<Scheduler>(logger_, url_database_, url_priority_list_)),
    storage_(std::make_shared<Storage>(logger_)),
    crawler_(std::make_shared<Crawler>(
      logger_, scheduler_, url_database_, url_priority_list_, storage_
    )) {
}

Injector::~Injector() {
  delete instance_;
}

Injector* Injector::Instance() {
  if (!instance_) { 
    instance_ = new Injector;
  }
  return instance_;
}

std::shared_ptr<ICrawler> Injector::crawler() {
  return crawler_;
}

std::shared_ptr<IUrlDatabase> Injector::url_database() {
  return url_database_;
}

std::shared_ptr<IUrlPriorityList> Injector::url_priority_list() {
  return url_priority_list_;
}

std::shared_ptr<IScheduler> Injector::scheduler() {
  return scheduler_;
}

} // End of namespace.
