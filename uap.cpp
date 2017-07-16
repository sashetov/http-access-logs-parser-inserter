#include <cassert>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <yaml-cpp/yaml.h>
#include "uap.hpp"

namespace {

  typedef std::map<std::string::size_type, size_t> i2tuple;

  struct GenericStore {
    std::string replacement;
    i2tuple replacementMap;
    boost::regex regExpr;
  };

  struct DeviceStore : GenericStore {
    std::string brandReplacement;
    std::string modelReplacement;
    i2tuple brandReplacementMap;
    i2tuple modelReplacementMap;
  };

  struct AgentStore : GenericStore {
    std::string majorVersionReplacement;
    std::string minorVersionReplacement;
    std::string patchVersionReplacement;
    std::string patchMinorVersionReplacement;
  };

  void mark_placeholders(i2tuple& replacement_map, const std::string& device_property) {
    auto loc = device_property.rfind("$");
    while (loc != std::string::npos) {
      const auto replacement = device_property.substr(loc + 1, 1);
      replacement_map[loc] = strtol(replacement.c_str(), nullptr, 10);

      if (loc < 2)
        break;

      loc = device_property.rfind("$", loc - 2);
    }
    return;
  }

  DeviceStore fill_device_store(const YAML::Node& device_parser) {
    DeviceStore device;
    bool regex_flag = false;
    //std::cout<< "======== fill device store\n";
    for (auto it = device_parser.begin(); it != device_parser.end(); ++it) {
      const auto key = it->first.as<std::string>();
      const auto value = it->second.as<std::string>();
      //std::cout<<"key: "<<key<<"\n";
      //std::cout<<"value: "<<key<<"\n";
      if (key == "regex") {
        device.regExpr.assign(value, boost::regex::optimize | boost::regex::normal);
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
      device.regExpr.assign(device.regExpr.str(), boost::regex::optimize | boost::regex::icase | boost::regex::normal);
    }
    return device;
  }

  AgentStore fill_agent_store(const YAML::Node& node,
                              const std::string& repl,
                              const std::string& major_repl,
                              const std::string& minor_repl,
                              const std::string& patch_repl) {
    AgentStore agent_store;
    //std::cout<< "======== fill agent store\n";
    assert(node.Type() == YAML::NodeType::Map);
    for (auto it = node.begin(); it != node.end(); ++it) {
      const auto key = it->first.as<std::string>();
      const auto value = it->second.as<std::string>();
      //std::cout<<"key "<<key<<"\n";
      //std::cout<<"value ''"<<value<<"'\n";
      if (key == "regex") {
        agent_store.regExpr.assign(value, boost::regex::optimize | boost::regex::normal);
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

  struct UAStore {
    explicit UAStore(const std::string& regexes_file_path) {
      auto regexes = YAML::LoadFile(regexes_file_path);

      const auto& user_agent_parsers = regexes["user_agent_parsers"];
      for (const auto& user_agent : user_agent_parsers) {
        const auto browser =
            fill_agent_store(user_agent, "family_replacement", "v1_replacement", "v2_replacement", "v3_replacement");
        browserStore.push_back(browser);
      }

      const auto& os_parsers = regexes["os_parsers"];
      for (const auto& o : os_parsers) {
        const auto os =
            fill_agent_store(o, "os_replacement", "os_v1_replacement", "os_v2_replacement", "os_v3_replacement");
        osStore.push_back(os);
      }

      const auto& device_parsers = regexes["device_parsers"];
      for (const auto& device_parser : device_parsers) {
        deviceStore.push_back(fill_device_store(device_parser));
      }
    }

    std::vector<DeviceStore> deviceStore;
    std::vector<AgentStore> osStore;
    std::vector<AgentStore> browserStore;
  };

  /////////////
  // HELPERS //
  /////////////

  void replace_all_placeholders(std::string& ua_property, const boost::smatch& result, const i2tuple& replacement_map) {
    for (auto iter = replacement_map.rbegin(); iter != replacement_map.rend(); ++iter) {
      ua_property.replace(iter->first, 2, result[iter->second].str());
    }
    boost::algorithm::trim(ua_property);
    return;
  }
  void print_all_matches( boost::smatch m ){
    unsigned int i =0;
    for(i=0; i< m.size(); i++ ){
      //std::cout<<"m["<<i<<"]: "<<m[i]<<"\n";
    }
  }
  Device parse_device_impl(const std::string& ua, const UAStore* ua_store) {
    Device device;
    //std::cout<< "======== parse device \n";
    for (const auto& d : ua_store->deviceStore) {
      boost::smatch m;
      if (boost::regex_search(ua, m, d.regExpr)) {
        //std::cout<<"d.regexp:"<<d.regExpr<<"\n";
        //print_all_matches(m);
        if (d.replacement.empty() && m.size() > 1) {
          device.family = m[1].str();
          //std::cout<< "device.family = m1"<< device.family << "\n";
        } else {
          device.family = d.replacement;
          //std::cout<< "device.family = device.replacement"<< device.family << "\n";
          //std::cout<<"d.replacementMap.empty? "<< std::boolalpha << d.replacementMap.empty() <<"\n";
          if (!d.replacementMap.empty()) {
            replace_all_placeholders(device.family, m, d.replacementMap);
            //std::cout<<" device.family(replace_all_placeholders) "<< device.family <<"\n";
          }
        }

        if (!d.brandReplacement.empty()) {
          device.brand = d.brandReplacement;
          //std::cout<< "device.brand = d.brandReplacement "<< d.brandReplacement<< "\n";
          //std::cout<<"d.replacementMap.empty? "<< std::boolalpha << d.replacementMap.empty() <<"\n";
          if (!d.brandReplacementMap.empty()) {
            replace_all_placeholders(device.brand, m, d.brandReplacementMap);
            //std::cout<<" device.brand(replace_all_placeholders) "<< device.brand <<"\n";
          }
        }
        if (d.modelReplacement.empty() && m.size() > 1) {
          device.model = m[1].str();
          //std::cout<< "device.model = m1 "<< m[1] << "\n";
        } else {
          device.model = d.modelReplacement;
          //std::cout<< "device.model = d.modelReplacement "<< d.modelReplacement << "\n";
          //std::cout<<"d.replacementMap.empty? "<< std::boolalpha << d.replacementMap.empty() <<"\n";
          if (!d.modelReplacementMap.empty()) {
            replace_all_placeholders(device.model, m, d.modelReplacementMap);
            //std::cout<<" device.model(replace_all_placeholders) "<< device.model <<"\n";
          }
        }
        break;
      } else {
        device.family = "Other";
      }
    }

    return device;
  }

  template <class AGENT, class AGENT_STORE >
  void fill_agent(AGENT& agent, const AGENT_STORE& store, const boost::smatch& m, const bool os) {
    //std::cout<< "======== fill agent (os="<< std::boolalpha<<os<<") \n";
    //std::cout<<"m.size(): "<<m.size()<<"\n";
    if (m.size() > 1) {
      //std::cout<<"store.replacement.empty "<< std::boolalpha << store.replacement.empty() <<"\n";
      agent.family =
          !store.replacement.empty() ? boost::regex_replace(store.replacement, boost::regex("\\$1"), m[1].str()) : m[1];
      //std::cout<<"store.replacement = "<<store.replacement<<"\n";
      //std::cout<<"m1 = "<<m[1].str()<<"\n";
      //std::cout<<"agent family = "<<agent.family<<"\n";
    } else {
      //std::cout<<"store.replacement.empty "<< std::boolalpha << store.replacement.empty() <<"\n";
      agent.family =
          !store.replacement.empty() ? boost::regex_replace(store.replacement, boost::regex("\\$1"), m[0].str()) : m[0];
      //std::cout<<"store.replacement = "<<store.replacement<<"\n";
      //std::cout<<"m0 = "<<m[0].str()<<"\n";
    }
    boost::algorithm::trim(agent.family);

    // The chunk above is slightly faster than the one below.
    // if ( store.replacement.empty() && m.size() > 1) {
    //   agent.family = m[1].str();
    // } else {
    //     agent.family = store.replacement;
    //     if ( ! store.replacementMap.empty()) {
    //       replace_all_placeholders(agent.family,m,store.replacementMap);
    //     }
    // }

    if (!store.majorVersionReplacement.empty()) {
      agent.major = store.majorVersionReplacement;
      //std::cout<<"store.majorVersionReplacement"<< store.majorVersionReplacement <<"\n";
    } else if (m.size() > 2) {
      //std::cout<<"m[2].str() agent.major = m2 = "<< m[2].str() <<"\n";
      agent.major = m[2].str();
    }
    if (!store.minorVersionReplacement.empty()) {
      agent.minor = store.minorVersionReplacement;
      //std::cout<<"store.minorVersionReplacement agent.minor= "<< store.minorVersionReplacement <<"\n";
    } else if (m.size() > 3) {
      agent.minor = m[3].str();
      //std::cout<<"agent.minor m3 "<< m[3] <<"\n";
    }
    if (!store.patchVersionReplacement.empty()) {
      agent.patch = store.patchVersionReplacement;
      //std::cout<<"agent.patch "<< store.patchVersionReplacement <<"\n";
    } else if (m.size() > 4) {
      agent.patch = m[4].str();
      //std::cout<<"agent.patch m4 "<< m[4] <<"\n";
    }
    if (os && m.size() > 5) {
      agent.patch_minor = m[5].str();
      //std::cout<<"agent.patch_minor m5 "<< m[5] <<"\n";
    }
  }

  Agent parse_browser_impl(const std::string& ua, const UAStore* ua_store) {
    //std::cout<<"parse browser\n";
    Agent browser;

    for (const auto& b : ua_store->browserStore) {
      boost::smatch m;
      if (boost::regex_search(ua, m, b.regExpr)) {
        //std::cout<<"b.regexp:"<<b.regExpr<<"\n";
        fill_agent(browser, b, m, false);
        break;
      } else {
        browser.family = "Other";
      }
    }

    return browser;
  }

  Agent parse_os_impl(const std::string& ua, const UAStore* ua_store) {
    //std::cout<<"parse os\n";
    Agent os;

    for (const auto& o : ua_store->osStore) {
      boost::smatch m;
      if (boost::regex_search(ua, m, o.regExpr)) {
        //std::cout<<"ua: "<<ua<<"\n";
        //std::cout<<"o.regexp:"<<o.regExpr<<"\n";
        fill_agent(os, o, m, true);
        break;
      } else {
        os.family = "Other";
      }
    }

    return os;
  }

}  // namespace
UserAgentParser::UserAgentParser(const std::string& regexes_file_path) : regexes_file_path_{regexes_file_path} {
  ua_store_ = new UAStore(regexes_file_path);
}
UserAgentParser::~UserAgentParser() {
  delete static_cast<const UAStore*>(ua_store_);
}
UserAgent UserAgentParser::parse(const std::string& ua) const {
  const auto ua_store = static_cast<const UAStore*>(ua_store_);
  const auto device = parse_device_impl(ua, ua_store);
  const auto os = parse_os_impl(ua, ua_store);
  const auto browser = parse_browser_impl(ua, ua_store);
  return {device, os, browser};
}

extern "C" void * uap_parser_create() {
  return new UserAgentParser ("./regexes_new.yaml");
}
extern "C" char * to_cstr( std::string str){
  char * charp = new char[str.size()+1];
  std::copy(str.begin(), str.end(), charp);
  charp[str.size()] = '\0';
  return charp;
}
extern "C" ua_agent_t convert_to_cagent( Agent agent ){
  ua_agent_t cagent;
  cagent.family = to_cstr( agent.family );
  cagent.major= to_cstr( agent.major );
  cagent.minor = to_cstr( agent.minor );
  cagent.patch = to_cstr( agent.patch );
  cagent.patch_minor = to_cstr( agent.patch_minor );
  return cagent;
}
extern "C" ua_device_t convert_to_cdevice( Device device ){
  ua_device_t cdevice;
  cdevice.family = to_cstr( device.family );
  cdevice.model = to_cstr( device.model );
  cdevice.brand = to_cstr( device.brand );
  return cdevice;
}
extern "C" ua_t convert_to_cua( UserAgent ua ){
  ua_t user_agent;
  user_agent.device = convert_to_cdevice(ua.device);
  user_agent.os = convert_to_cagent( ua.os );
  user_agent.browser  = convert_to_cagent( ua.browser);
  return user_agent;
}
extern "C" ua_t * parse_to_c_ua( char * ua_cstr ) {
  const UserAgentParser g_ua_parser("./uap_regexes.yaml");
  std::string uastr(ua_cstr);
  UserAgent ua = g_ua_parser.parse( uastr );
  ua_t uat = convert_to_cua(ua);
  return &uat;
}
