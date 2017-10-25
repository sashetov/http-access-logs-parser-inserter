#include "htlog_uap.hpp"
void mark_placeholders(std::map<std::string::size_type, size_t>& replacement_map, const std::string device_property) {
  std::size_t loc = device_property.rfind("$");// finds start of replacement param $1, $2 ... etc
  while ( loc != std::string::npos ) {
    const std::string repl_number = device_property.substr(loc + 1, 1);
    replacement_map[loc] = strtol(repl_number.c_str(), nullptr, 10);
    if (loc < 2) break; // skip any unnecessary rfinds..
    loc = device_property.rfind("$", loc - 2);
  }
  return;
}
AgentStore fill_agent_store( const YAML::Node node, const std::string repl, const std::string major_repl, const std::string minor_repl, const std::string patch_repl) {
  AgentStore agent_store;
  assert(node.Type() == YAML::NodeType::Map);
  for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
    const std::string key = it->first.as<std::string>();
    const std::string value = it->second.as<std::string>();
    if (key == "regex") {
      agent_store.regExpr.re_str = value;
      agent_store.regExpr.re.assign(value, boost::regex::optimize | boost::regex::normal);
      /*try {
        agent_store.regExpr.re = std::regex( value, std::regex::optimize | std::regex::ECMAScript );
      } catch (std::regex_error& e) {
        print_regexp_error(agent_store.regExpr.re_str,e);
      }*/
    } else if (key == repl) {
      agent_store.replacement = value;
      mark_placeholders(agent_store.replacementMap, agent_store.replacement);
    } else if (key == major_repl && !value.empty()) {
      agent_store.majorVersionReplacement = value;
    } else if (key == minor_repl && !value.empty()) {
      agent_store.minorVersionReplacement = value;
    } else if (key == patch_repl && !value.empty()) {
      agent_store.patchVersionReplacement = value;
    } else {
      assert(false);
    }
  }
  return agent_store;
}
DeviceStore fill_device_store(const YAML::Node& device_parser) {
  DeviceStore device;
  bool regex_flag = false;
  for (YAML::const_iterator it= device_parser.begin(); it != device_parser.end(); ++it) {
    const std::string key = it->first.as<std::string>();
    const std::string value = it->second.as<std::string>();
    if (key == "regex") {
      device.regExpr.re_str = value;
      device.regExpr.re.assign(value, boost::regex::optimize | boost::regex::normal);
      /*try {
        device.regExpr.re = std::regex( value, std::regex::optimize | std::regex::ECMAScript );
      } catch (std::regex_error& e) {
        print_regexp_error(device.regExpr.re_str,e);
      }*/
    } else if (key == "regex_flag" && value == "i") {
      regex_flag = true;
    } else if (key == "device_replacement") {
      device.replacement = value;
      mark_placeholders(device.replacementMap, device.replacement);
    } else if (key == "model_replacement") {
      device.modelReplacement = value;
      mark_placeholders(device.modelReplacementMap, device.modelReplacement);
    } else if (key == "brand_replacement") {
      device.brandReplacement = value;
      mark_placeholders(device.brandReplacementMap, device.brandReplacement);
    } else {
      assert(false);
    }
  }
  if (regex_flag == true) {
    device.regExpr.re.assign(device.regExpr.re_str, boost::regex::optimize | boost::regex::icase | boost::regex::normal);
    /*try {
      device.regExpr.re = std::regex(device.regExpr.re_str, std::regex::optimize | std::regex::icase | std::regex::ECMAScript );
    } catch (std::regex_error& e) {
      print_regexp_error(device.regExpr.re_str,e);
    }*/
  }
  return device;
}
UAStore::UAStore( std::string regexes_file_path ){
  const YAML::Node regexes = YAML::LoadFile( regexes_file_path );
  const YAML::Node user_agent_parsers = regexes["user_agent_parsers"];
  for (const YAML::Node user_agent : user_agent_parsers) {
    AgentStore browser = fill_agent_store( user_agent, "family_replacement", "v1_replacement", "v2_replacement", "v3_replacement" );
    browserStore.push_back(browser);
  }
  const YAML::Node os_parsers = regexes["os_parsers"];
  for (const YAML::Node o : os_parsers) {
    AgentStore os = fill_agent_store( o, "os_replacement", "os_v1_replacement", "os_v2_replacement", "os_v3_replacement");
    osStore.push_back(os);
  }
  const YAML::Node device_parsers = regexes["device_parsers"];
  for (const YAML::Node device_parser : device_parsers) {
    deviceStore.push_back(fill_device_store(device_parser));
  }
}
UAStore::~UAStore(){
}
void trim(std::string& str) {
  size_t first = str.find_first_not_of(' ');
  if (std::string::npos == first) {
    return;
  }
  size_t last = str.find_last_not_of(' ');
  str = str.substr(first, (last - first + 1));
}
//void replace_all_placeholders( std::string& ua_property, const std::smatch& result, std::map<std::string::size_type, size_t> replacement_map) {
void replace_all_placeholders( std::string& ua_property, const boost::smatch& result, std::map<std::string::size_type, size_t> replacement_map) {
  for ( std::map<std::string::size_type, size_t>::reverse_iterator iter = replacement_map.rbegin(); iter != replacement_map.rend(); ++iter ) {
    ua_property.replace(iter->first, 2, result[iter->second].str());
  }
  trim(ua_property);
  return;
}
Device parse_device_impl(const std::string& ua, const UAStore* ua_store) {
  Device device;
  for (const auto& d : ua_store->deviceStore) {
    //std::smatch m;
    //if ( std::regex_search(ua, m, d.regExpr.re)) {
    boost::smatch m;
    if ( boost::regex_search(ua, m, d.regExpr.re)) {
      if (d.replacement.empty() && m.size() > 1) {
        device.family = m[1].str();
      } else {
        device.family = d.replacement;
        if (!d.replacementMap.empty()) {
          replace_all_placeholders( device.family, m, d.replacementMap );
        }
      }
      if (!d.brandReplacement.empty()) {
        device.brand = d.brandReplacement;
        if (!d.brandReplacementMap.empty()) {
          replace_all_placeholders(device.brand, m, d.brandReplacementMap);
        }
      }
      if (d.modelReplacement.empty() && m.size() > 1) {
        device.model = m[1].str();
      } else {
        device.model = d.modelReplacement;
        if (!d.modelReplacementMap.empty()) {
          replace_all_placeholders(device.model, m, d.modelReplacementMap);
        }
      }
      break;
    } else {
      device.family = "Other";
    }
  }
  return device;
}
//template <class AGENT, class AGENT_STORE> void fill_agent( AGENT& agent, const AGENT_STORE& store, const std::smatch& m, const bool os ) {
template <class AGENT, class AGENT_STORE> void fill_agent( AGENT& agent, const AGENT_STORE& store, const boost::smatch& m, const bool os ) {
  if (m.size() > 1) {
    agent.family = !store.replacement.empty() ?  std::regex_replace( store.replacement, std::regex("\\$1"), m[1].str()) : m[1];
  } else {
    agent.family = !store.replacement.empty() ?  std::regex_replace( store.replacement, std::regex("\\$1"), m[0].str()) : m[0];
    trim(agent.family);
  }
  if (!store.majorVersionReplacement.empty()) {
    agent.major = store.majorVersionReplacement;
  } else if (m.size() > 2) {
    agent.major = m[2].str();
  }
  if (!store.minorVersionReplacement.empty()) {
    agent.minor = store.minorVersionReplacement;
  } else if (m.size() > 3) {
    agent.minor = m[3].str();
  }
  if (!store.patchVersionReplacement.empty()) {
    agent.patch = store.patchVersionReplacement;
  } else if (m.size() > 4) {
    agent.patch = m[4].str();
  }
  if (os && m.size() > 5) {
    agent.patch_minor = m[5].str();
  }
}
Agent parse_browser_impl(const std::string& ua, const UAStore* ua_store) {
  Agent browser;
  for (const auto& b : ua_store->browserStore) {
    //std::smatch m;
    //if (std::regex_search(ua, m, b.regExpr.re)) {
    boost::smatch m;
    if (boost::regex_search(ua, m, b.regExpr.re)) {
      fill_agent(browser, b, m, false);
      break;
    } else {
      browser.family = "Other";
    }
  }
  return browser;
}
Agent parse_os_impl(const std::string& ua, const UAStore* ua_store) {
  Agent os;
  for (const auto& o : ua_store->osStore) {
    //std::smatch m;
    //if (std::regex_search(ua, m, o.regExpr.re)) {
    boost::smatch m;
    if (boost::regex_search(ua, m, o.regExpr.re)) {
      fill_agent(os, o, m, true);
      break;
    } else {
      os.family = "Other";
    }
  }
  return os;
}
UserAgentParser::UserAgentParser(std::string regexes_file_path) : regexes_file_path_(regexes_file_path) {
  ua_store_ = new UAStore(regexes_file_path);
}
UserAgentParser::~UserAgentParser() {
  delete static_cast<const UAStore*>(ua_store_);
}
UserAgent UserAgentParser::parse(std::string ua) const {
  const UAStore * ua_store = static_cast<const UAStore*>(ua_store_);
  Device device = parse_device_impl(ua, ua_store);
  Agent os = parse_os_impl(ua, ua_store);
  Agent browser = parse_browser_impl(ua, ua_store);
  return {device, os, browser};
}
