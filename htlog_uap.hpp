#ifndef __HTLOG_UAP__
#include <string>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
//#include <regex>
#include <yaml-cpp/yaml.h>
struct RegexContainer {
  std::string re_str;
  boost::regex  re;
  //std::regex re;
};
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
struct GenericStore {
  std::string replacement;
  std::map<std::string::size_type, size_t> replacementMap;
  RegexContainer regExpr;
};
struct DeviceStore : GenericStore {
  std::string brandReplacement;
  std::string modelReplacement;
  std::map<std::string::size_type, size_t> brandReplacementMap;
  std::map<std::string::size_type, size_t> modelReplacementMap;
};
struct AgentStore : GenericStore {
  std::string majorVersionReplacement;
  std::string minorVersionReplacement;
  std::string patchVersionReplacement;
  std::string patchMinorVersionReplacement;
};
struct UserAgent {
  Device device;
  Agent os;
  Agent browser;
  std::string toFullString() const { return browser.toString() + "/" + os.toString(); }
  bool isSpider() const { return device.family == "Spider"; }
};
void mark_placeholders(std::map<std::string::size_type, size_t>&, const std::string);
void print_regexp_error( std::string,std::regex_error );
AgentStore fill_agent_store( const YAML::Node, const std::string, const std::string, const std::string, const std::string);
DeviceStore fill_device_store(const YAML::Node& device_parser);
class UAStore {
  public:
  UAStore( std::string ) ;
  ~UAStore();
  std::vector<DeviceStore> deviceStore;
  std::vector<AgentStore> osStore;
  std::vector<AgentStore> browserStore;
};
void trim(std::string& str);
//void replace_all_placeholders( std::string&, const std::smatch&, std::map<std::string::size_type, size_t>);
void replace_all_placeholders( std::string&, const boost::smatch&, std::map<std::string::size_type, size_t>);
Device parse_device_impl(const std::string&, const UAStore* );
//template <class AGENT, class AGENT_STORE> void fill_agent( AGENT& , const AGENT_STORE& , const std::smatch& , const bool );
template <class AGENT, class AGENT_STORE> void fill_agent( AGENT& , const AGENT_STORE& , const boost::smatch& , const bool );
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
#define __HTLOG_UAP__
#endif
