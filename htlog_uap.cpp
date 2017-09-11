#include "htlog_uap.hpp"
// UAP
UserAgentParser::UserAgentParser(const std::string& regexes_file_path) : regexes_file_path_{regexes_file_path} {
  ua_store_ = (const void *) new UAStore(regexes_file_path);
}
UserAgentParser::~UserAgentParser() {
  delete (ua_store_);
}
UserAgent UserAgentParser::parse(const std::string& ua) const {
  const UAStore * ua_store = (const UAStore*)(ua_store_);
  Device device = parse_device_impl(ua, ua_store);
  Agent os = parse_os_impl(ua, ua_store);
  Agent browser = parse_browser_impl(ua, ua_store);
  return {device, os, browser};
}
