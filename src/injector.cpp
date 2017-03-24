#include "injector.hpp"

namespace Crawler {

Injector* Injector::instance_ = 0;

Injector::Injector() 
  : ck_spider_(std::make_shared<CkSpider>()),
    logger_(std::make_shared<Logger>()),
    scheduler_(std::make_shared<Scheduler>(logger_)),
    fetcher_(std::make_shared<Fetcher>(logger_, ck_spider_)),
    storage_(std::make_shared<Storage>(logger_)),
    url_database_(std::make_shared<UrlDatabase>(logger_)),
    crawler_(std::make_shared<Crawler>(logger_, scheduler_, fetcher_, storage_)) {
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

} // End of namespace.
