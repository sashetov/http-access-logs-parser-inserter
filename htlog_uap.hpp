#ifndef __HTLOG_UAP__
#include <string>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <regex>
#include <yaml-cpp/yaml.h>
void trim(std::string& str);
void mark_placeholders(std::map<std::string::size_type, size_t>&, const std::string);
void replace_all_placeholders( std::string&, const boost::smatch&, std::map<std::string::size_type, size_t>);
template <class A, class AS> void fill_agent( A& , const AS& , const boost::smatch& , const bool );
struct RegexContainer {
  std::string re_str;
  boost::regex  re; //std::regex re;
};
struct GenericStore {
  std::string replacement;
  std::map<std::string::size_type, size_t> replacementMap;
  RegexContainer regExpr;
};
struct AgentStore : GenericStore {
  std::string majorVersionReplacement;
  std::string minorVersionReplacement;
  std::string patchVersionReplacement;
  std::string patchMinorVersionReplacement;
};
AgentStore fill_agent_store( const YAML::Node, const std::string, const std::string, const std::string, const std::string);
struct DeviceStore : GenericStore {
  std::string brandReplacement;
  std::string modelReplacement;
  std::map<std::string::size_type, size_t> brandReplacementMap;
  std::map<std::string::size_type, size_t> modelReplacementMap;
};
DeviceStore fill_device_store(const YAML::Node& device_parser);
struct Generic {
  std::string family;
};
struct Device : Generic {
  std::string model;
  std::string brand;
  std::string toString() const { return model + " " + brand; };
};
struct Agent : Generic {
  std::string major;
  std::string minor;
  std::string patch;
  std::string patch_minor;
  std::string toString() const { return family + " " + toVersionString(); }
  std::string toVersionString() const {
    return (major.empty() ? "0" : major) + "." + (minor.empty() ? "0" : minor) + "." + (patch.empty() ? "0" : patch);
  }
};
struct UserAgent {
  Device device;
  Agent os;
  Agent browser;
  std::string toFullString() const { return browser.toString() + "/" + os.toString(); }
  bool isSpider() const { return device.family == "Spider"; }
};
class UAStore {
  public:
  UAStore( std::string ) ;
  ~UAStore();
  std::vector<DeviceStore> deviceStore;
  std::vector<AgentStore> osStore;
  std::vector<AgentStore> browserStore;
};
Device parse_device_impl(const std::string&, const UAStore* );
Agent parse_browser_impl(const std::string& ua, const UAStore* ua_store);
Agent parse_os_impl(const std::string& ua, const UAStore* ua_store);
class UserAgentParser {
 public:
  UserAgentParser(std::string );
  UserAgent parse(std::string) const;
  ~UserAgentParser();
 private:
  const std::string regexes_file_path_;
  const void* ua_store_;
};
const UserAgentParser g_ua_parser("./uap_regexes.yaml");
#define __HTLOG_UAP__
#endif
