#include <stdio.h>
#include <time.h>
#include <map>
#include <vector>
#include <thread>
#include <fstream>
#include <arpa/inet.h>
#include <iostream>
#include <GeoLite2PP.hpp>
#include <cassert>
#include <cstdlib>
#include <string>
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
      if (loc < 2){
        break;
      }
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
  AgentStore fill_agent_store(
    const YAML::Node& node,
    const std::string& repl, const std::string& major_repl,
    const std::string& minor_repl, const std::string& patch_repl) {
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
      YAML::Node regexes = YAML::LoadFile(regexes_file_path);
      const auto& user_agent_parsers = regexes["user_agent_parsers"];
      for (const YAML::Node &user_agent : user_agent_parsers) {
        AgentStore browser = fill_agent_store( user_agent,
              "family_replacement",
              "v1_replacement", 
              "v2_replacement",
              "v3_replacement" );
        browserStore.push_back(browser);
      }
      const auto& os_parsers = regexes["os_parsers"];
      for (const auto& o : os_parsers) {
        const auto os = fill_agent_store( o,
            "os_replacement", "os_v1_replacement",
            "os_v2_replacement", "os_v3_replacement");
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
      std::cout<<"m["<<i<<"]: "<<m[i]<<"\n";
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
        //std::cout<<"device.family: "<<device.family<<" "<<"device.brand: "<<device.brand<<" "<<"device.model: "<<device.model<<" \n";
        break;
      } else {
        device.family = "Other";
      }
    }
    return device;
  }
  template <class AGENT, class AGENT_STORE> void fill_agent( AGENT& agent,
      const AGENT_STORE& store,
      const boost::smatch& m, const bool os ) {
    //std::cout<< "======== fill agent (os="<< std::boolalpha<<os<<") \n";
    //std::cout<<"m.size(): "<<m.size()<<"\n";
    if (m.size() > 1) {
      //std::cout<<"store.replacement.empty "<< std::boolalpha << store.replacement.empty() <<"\n";
      agent.family = !store.replacement.empty() ? 
        boost::regex_replace(
            store.replacement, boost::regex("\\$1"), m[1].str()) : m[1].str();
      //std::cout<<"store.replacement = "<<store.replacement<<"\n";
      //std::cout<<"m1 = "<<m[1].str()<<"\n";
      //std::cout<<"agent family = "<<agent.family<<"\n";
    } else {
      //std::cout<<"store.replacement.empty "<< std::boolalpha << store.replacement.empty() <<"\n";
      agent.family = !store.replacement.empty() ?
        boost::regex_replace(
            store.replacement, boost::regex("\\$1"), m[0].str()) : m[0].str();
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
  const UserAgentParser g_ua_parser("./uap_regexes.yaml");
}  // namespace
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
typedef struct logline {
  std::string  hostname;
  unsigned long  userIP;
  std::string userIPStr;
  std::string  date;
  unsigned long timestamp;
  std::string  requestURL;
  std::string  requestType;
  std::string  referrer;
  std::string  agent;
  long sizeBytes;
  int statusCode;
} parsed_logline;
class KeyValueContainer{
  public:
    KeyValueContainer( std::string , std::string );
    std::string getKey() const;
    std::string getValue() const;
    ~KeyValueContainer();
    int operator <( const KeyValueContainer & ) const;
  private:
    std::string key;
    std::string value;
};
KeyValueContainer::KeyValueContainer( std::string k, std::string v) {
  key = k;
  value = v;
}
std::string KeyValueContainer::getKey() const{
  return key;
}
std::string KeyValueContainer::getValue() const {
  return value;
}
int KeyValueContainer::operator <( const KeyValueContainer &rhs) const {
  return key < rhs.getKey() && value < rhs.getValue();
}
KeyValueContainer::~KeyValueContainer() {
}
class HttpAccessLogMetrics {
  public:
    HttpAccessLogMetrics( int, int, std::vector<std::string>, std::vector<std::string>,std::string);
    ~HttpAccessLogMetrics();
    int logsScanParallel(int,int,long);
    void parseLogFile(int);
    int parseLine(std::string,parsed_logline &);
    unsigned long getNumericIp( std::string );
    unsigned long getTimestamp( std::string );
    template<typename T> void incrementCount( std::map<T,int>*, T );
    void processUserAgent( std::string );
    void processRefererStrings( std::string );
    std::string getCountryFromIP( std::string );
    std::vector<KeyValueContainer> parseParamsString( std::string );
  private:
    std::vector<std::thread*> threads;
    int st; // start timestamp
    int et; // end timestamp
    int real_did;
    int uid;
    int lines_failed;
    int lines_processed;
    std::string filename;
    std::vector<std::string> internal_hostnames;
    std::vector<std::string> search_hostnames;
    std::string* error;
    std::map<unsigned long, int> client_ips;                   // ip_id
    std::map<std::string,int> client_geo_locations;         // country_id
    std::map<std::string,int> client_devices;               // with client_devices vers device_id 
    std::map<std::string,int> client_oses;                  // os_id
    std::map<std::string,int> client_browsers;              // browser_id
    std::map<std::string,int> page_paths;                   // page_id, needs domains_id
    std::map<std::string,int> referer_hostnames;            // referer_domain_id
    std::map<std::string,int> referer_pathstrings;          // with referer_hostnames: referer_id, referer_domain_id
    std::map<KeyValueContainer,int> referer_params;         // with internref_hostnames, internref_pathstrings, internref_params: domain_id, page_id, url_param_id
    std::map<std::string,int> internref_hostnames;          // domain_id
    std::map<std::string,int> internref_pathstrings;        // page_id
    std::map<KeyValueContainer,int> internref_params;       // url_param_id
    std::map<std::string,int> search_queries;               // search_term_id
    std::map<std::string,int> tvectors_inner;               // needs page_id
    std::map<std::string,int> tvectors_inner_per_hour;      // needs page_id
    std::map<std::string,int> tvectors_incoming;            // needs referer_id
    std::map<std::string,int> tvectors_inc_per_hour;        // needs referer_id
    std::map<std::string,int> hits;                         // needs page_id
    std::map<std::string,int> visits;                       // needs ip_id entities
    std::map<std::string,int> pageviews;                    // needs page_id, ip_id
    //methods
    int getLinesNumber();
};
HttpAccessLogMetrics::HttpAccessLogMetrics( int did, int user_id, std::vector<std::string> user_hosts, std::vector<std::string> search_hosts, std::string file ){
  st = time(NULL);
  real_did = did;
  uid = user_id;
  int i =0;
  lines_failed=0;
  lines_processed=0;
  for( i =0; i< (int)user_hosts.size(); i++){
    internal_hostnames.push_back( user_hosts[i]);
  }
  for( i =0; i< (int)search_hosts.size(); i++){
    search_hostnames.push_back( search_hosts[i]);
  }
  filename = file;
}
HttpAccessLogMetrics::~HttpAccessLogMetrics(){
}
int HttpAccessLogMetrics::logsScanParallel( int threadNumber, int linesNumber, long startLine ){
  std::ifstream statsFile;
  int i = 0;
  std::string line;
  try {
    statsFile.exceptions( std::ifstream::badbit | std::ifstream::failbit);
    /*std::string threadLogString = "./logs/thread-"+std::to_string(threadNumber)+".log";
    std::ofstream threadLogFile;
    threadLogFile.open(threadLogString);
    threadLogFile
      <<"threadNumber:"<<threadNumber<<"\n"
      <<"linesNumber:"<<linesNumber<<"\n"
      <<"startLine:"<<startLine<<"\n";*/
    // don't use [] operator to check for \0, as it will segfault sometimes... ( BU WHYYYYYYYYY?)
    if (filename == "-") {
      return 1;
    }
    statsFile.open(filename);
    while (std::getline(statsFile, line) && i < startLine ) { 
      i++;
    }
    i=0;
    while (!statsFile.eof() && i < linesNumber){
      std::getline(statsFile, line);
      //threadLogFile<<"tid:"<<tid<<" "<<filename<<":"<<(i+startLine)<<"\n"<<line;
      parsed_logline ll;
      if (parseLine(line,ll) == 0) {
        incrementCount( &client_ips, ll.userIP);
        incrementCount( &client_geo_locations, getCountryFromIP(ll.userIPStr));
        processUserAgent( ll.agent );
        processRefererStrings( ll.referrer );
        lines_processed++;
      }
      else {
        lines_failed++;
        std::cerr<<"Invalid line: "<< line<<"\n";
      }
      i++;
    }
    statsFile.close();
  } catch (std::exception e) {
    if( !statsFile.eof()){
      std::cerr
        << "Exception opening/reading "
        <<filename<<":"
        <<i+startLine<<"\n"
        <<line<<"\n";
      return 3;
    }
  }
  et = time(NULL);
  //threadLogFile.close();
  return 0;
}
int HttpAccessLogMetrics::getLinesNumber(){
  int i =0;
  std::string line;
  std::ifstream statsFile(filename);
  while(std::getline(statsFile, line)){
    i++;
  }
  return i;
}
void HttpAccessLogMetrics::parseLogFile( int numThreads ){
  int i =0;
  int lines = getLinesNumber();
  int remainder = lines % numThreads;
  int chunkLines = lines / numThreads;
  long lineIndex = 0;
  int threadNumber = 0;
  for( i= 0; i < numThreads ; i++ ){
    int numLines = chunkLines;
    if( i == numThreads -1 ){
      numLines += remainder;
    }
    threads.push_back( new std::thread( 
          &HttpAccessLogMetrics::logsScanParallel, this,
          threadNumber, numLines, lineIndex ));
    lineIndex += numLines;
    threadNumber++;
  }
  for( i =0; i< numThreads; i++){
    threads[i]->join();
  }
}
unsigned long HttpAccessLogMetrics::getNumericIp( std::string addr ){
  uint32_t nbo_addr = inet_addr(addr.c_str());//address in network byte order
  return (unsigned long) ntohl(nbo_addr);
}
unsigned long HttpAccessLogMetrics::getTimestamp( std::string dateTime ){
  struct tm tm;
  time_t epoch;
  if ( strptime(dateTime.c_str(), "%d/%h/%Y:%H:%M:%S %Z", &tm) != NULL ){
    epoch = mktime(&tm);
  }
  else {
    return -1;
  }
  return (unsigned long) epoch;
}
int HttpAccessLogMetrics::parseLine( std::string line, parsed_logline &ll ){
  //char *user_hostname, *date, *hour, *timezone, *host, *agent, *req, *ref, *p;
  std::string host, userIP, 
    date, hour, timezone,
    request, requestType, requestURL, requestProtocol,
    referrer,
    userAgent,
    size,
    status;
  int statusCode;
  long sizeBytes;
  int uIPCursor0, uIPCursor1,
      dateCursor0, dateCursor1,
      reqCursor0, reqCursor1,
      reqUrlCursor0, reqUrlCursor1,
      refCursor0, refCursor1,
      uaCursor0, uaCursor1,
      statusCursor0, statusCursor1,
      sizeCursor0, sizeCursor1
      ;
  //char *agent_start = NULL, *req_end = NULL, *ref_end = NULL;
  int agentIndex = 0, reqEnd = 0, refEnd = 0, agentWithoutParentheses= 0;
  //USER IP
  if ((uIPCursor0= line.find_first_of(" ")) == -1){
    return 1;
  }
  //HOST
  host = line.substr( 0, uIPCursor0 );
  if ((uIPCursor1= line.find_first_of(" ", uIPCursor0+1) )== -1){
    return 2;
  }
  userIP = line.substr(uIPCursor0+1,uIPCursor1-uIPCursor0);
  //DATE
  if ((dateCursor0 = line.find_first_of("[")) == -1){
    return 3;
  }
  if ((dateCursor1 = line.find_first_of("]",dateCursor0+1)) == -1){
    return 4;
  }
  date = line.substr( dateCursor0+1, dateCursor1-dateCursor0-1 );
  //REQ
  if ((reqCursor0 = line.find_first_of("\"")) == -1){
    return 5;
  }
  if ((reqCursor1 = line.find_first_of("\"",reqCursor0+1)) == -1){
    return 6;
  }
  request = line.substr( reqCursor0+1, reqCursor1-reqCursor0-1 );
  if ((reqUrlCursor0 = request.find_first_of(" ")) == -1){
    return 7;
  }
  if ((reqUrlCursor1= request.find_last_of(" ")) == -1){
    return 8;
  }
  requestURL = request.substr(reqUrlCursor0+1, reqUrlCursor1 -reqUrlCursor0 -1);
  requestType = request.substr(0,reqUrlCursor0);
  requestProtocol= request.substr(reqUrlCursor1+1, request.size()-reqUrlCursor1-1);
  //REFERRER
  if ((refCursor0= line.find_first_of("\"", reqCursor1+1)) == -1){
    return 9;
  }
  if ((refCursor1= line.find_first_of("\"",refCursor0+1)) == -1){
    return 10;
  }
  referrer= line.substr( refCursor0+1, refCursor1-refCursor0-1 );
  //USER AGENT
  if ((uaCursor0= line.find_first_of("\"", refCursor1+1)) == -1){
    return 11;
  }
  if ((uaCursor1= line.find_first_of("\"", uaCursor0+1)) == -1){
    return 12;
  }
  userAgent = line.substr(uaCursor0+1, uaCursor1-uaCursor0-1);
  //STATUSCODE
  if ((statusCursor0= line.find_first_of(" ", reqCursor1+1)) == -1){
    return 13;
  }
  if ((statusCursor1= line.find_first_of(" ", statusCursor0+1)) == -1){
    return 14;
  }
  status = line.substr(statusCursor0+1, statusCursor1-statusCursor0-1);
  statusCode = std::stoi(status);
  //SIZE IN BYTES
  if ((sizeCursor0 = line.find_first_of(" ", statusCursor1)) == -1){
    return 15;
  }
  if ((sizeCursor1 = line.find_first_of(" ", sizeCursor0+1)) == -1){
    return 16;
  }
  size= line.substr(sizeCursor0+1, sizeCursor1-sizeCursor0-1);
  sizeBytes = std::stol(size);
  ll.hostname = host;
  ll.userIPStr = userIP;
  ll.userIP= getNumericIp(userIP);
  ll.date= date;
  ll.timestamp = getTimestamp(date);
  //ll->hour= host;
  //ll->timezone= timezone;
  ll.requestURL = requestURL;
  ll.requestType = requestType;
  ll.referrer = referrer;
  ll.agent = userAgent;
  ll.sizeBytes = sizeBytes;
  ll.statusCode= statusCode;
  /*std::cout
    <<"hostname:"<<host<<"\n"
    <<"userIP:"<<userIP<<"\n"
    <<"date:"<<date<<"\n"
    <<"request:"<<request<<"\n"
    <<"requestURL:"<<requestURL<<"\n"
    <<"requestType:"<<requestType<<"\n"
    <<"requestProtocol:"<<requestProtocol<<"\n"
    <<"referrer:"<<referrer<<"\n"
    <<"userAgent:"<<userAgent<<"\n"
    <<"statusCode:"<<statusCode<<"\n"
    <<"sizeBytes:"<<sizeBytes<<"\n"
    ;*/
  /*std::cout
    <<"ll->hostname:"<<ll.hostname<<"\n"
    <<"userIP:"<<userIP<<"\n"
    <<"ll->userIP:"<<ll.userIP<<"\n"
    <<"ll->date:"<<ll.date<<"\n"
    <<"ll->timestamp:"<<ll.timestamp<<"\n"
    <<"ll->requestURL:"<<ll.requestURL<<"\n"
    <<"ll->requestType:"<<ll.requestType<<"\n"
    <<"ll->referrer:"<<ll.referrer<<"\n"
    <<"ll->agent:"<<ll.agent<<"\n"
    <<"ll->sizeBytes:"<<ll.sizeBytes<<"\n"
    <<"ll->statusCode:"<<ll.statusCode<<"\n"
    ;*/
  return 0;
}
template<typename T> void HttpAccessLogMetrics::incrementCount( std::map<T,int> *kvmap, T key ){
  int count=1;
  typename std::map<T, int>::iterator it = kvmap->find(key);
  if (it == kvmap->end()){ //nothing was found
    kvmap->insert(std::make_pair(key, count));
  }
  else {
    it->second += count;
  }
}
std::string HttpAccessLogMetrics::getCountryFromIP( std::string client_ip){
  GeoLite2PP::DB db( "GeoLite2-City.mmdb" );
  GeoLite2PP::MStr m = db.get_all_fields( client_ip );
  return m["country_iso_code"];
}
void HttpAccessLogMetrics::processUserAgent( std::string uaStr ){
  const UserAgent ua = g_ua_parser.parse( uaStr );
  incrementCount( &client_devices, (std::string) ua.device.toString() );
  incrementCount( &client_oses, (std::string)ua.os.toString() );
  incrementCount( &client_browsers, (std::string)ua.browser.toString() );
}
std::vector<KeyValueContainer> HttpAccessLogMetrics::parseParamsString( std::string params_str ){
  std::vector<KeyValueContainer> results;
  std::size_t found;
  if( params_str.length() > 0 ) { 
    std::string workstring(params_str);
    std::string param;
    while((found = workstring.find("&") )!= -1 ){
      std::string query;
      param = workstring.substr(0,found);
      workstring = workstring.substr(found+1, workstring.length()-found-1);
      if((found = param.find("=")) != -1 ){
        std::string key = param.substr(0,found);
        std::string val = param.substr(found+1, param.length()-found-1);
        results.push_back(KeyValueContainer(key,val));
      }
    }
    param = workstring;
    if((found = param.find("=")) != -1 ){
      std::string key = param.substr(0,found);
      std::string val = param.substr(found+1, param.length()-found-1);
      results.push_back(KeyValueContainer(key,val));
    }
  }
  return results;
}
void HttpAccessLogMetrics::processRefererStrings( std::string referer ){
  std::vector<std::string>::iterator it;
  std::vector<KeyValueContainer>::iterator params_it;
  int hostname_type= 0; // 0 nothing found, 1 found internal ref, 2 found external ref, 3 found search ref
  std::string path_noproto, hostname, page_path, params_str;
  std::vector<KeyValueContainer> params;
  std::size_t found = referer.find("://");
  if(found !=-1){
    path_noproto = referer.substr(found+3, referer.length()-found);
  }
  else {
    path_noproto = "";
    hostname = "";
    page_path = "";
  }
  found = path_noproto.find("/");
  if(found == -1 ){
    hostname = path_noproto;
    page_path = "";
  }
  else{
    hostname = path_noproto.substr(0,found);
    page_path = path_noproto.substr(found+1, path_noproto.length() - found );
  }
  found = page_path.find("?");
  if(found != -1){
    params_str = page_path.substr(found+1, page_path.length() - found);
  }
  for (  it = internal_hostnames.begin(); it != internal_hostnames.end(); it++ ) {
    if( hostname == *it){
      hostname_type = 1;
      break;
    }
    else {
      hostname_type = 2;
    }
  }
  for (  it = search_hostnames.begin(); it != search_hostnames.end(); it++ ) {
    if( hostname == *it ){
      hostname_type = 3;
      break;
    }
  }
  if(params_str.length() > 0 ) {
     params = parseParamsString( params_str );
  }
  if(hostname_type == 1 ) {
    if( hostname.length() > 0 ) {
      //std::cout<<"internref_hostname '"<<hostname<<"'\n";
      incrementCount(&internref_hostnames, hostname);
    }
    if( page_path.length() > 0 ) {
      //std::cout<<"internref_pathstring: '"<<page_path<<"'\n";
      incrementCount(&internref_pathstrings, page_path);
    }
    if( params_str.length() > 0) {
      //std::cout<<"internref_paramstring: '"<<params_str<<"'\n";
      for (  params_it = params.begin() ; params_it != params.end(); params_it++){
        std::cout<<"internref_param key '"<<(*params_it).getKey()<<"'\n";
        std::cout<<"internref_param value '"<<(*params_it).getValue()<<"'\n";
        incrementCount(&internref_params, (*params_it));
      }
    }
  }
  else if(hostname_type == 2 ) {
    if( hostname.length() > 0 ) {
      //std::cout<<"referer_hostname '"<<hostname<<"'\n";
      incrementCount(&referer_hostnames, hostname);
    }
    if( page_path.length() > 0 ) {
      //std::cout<<"referer_pathstring: '"<<page_path<<"'\n";
      incrementCount(&referer_pathstrings, page_path);
    }
    if( params_str.length() > 0) {
      //std::cout<<"referer_paramstring: '"<<params_str<<"'\n";
      for( params_it = params.begin(); params_it!=params.end(); params_it++){
        //std::cout<<"referer_param key '"<<(*params_it).getKey()<<"'\n";
        //std::cout<<"referer_param value '"<<(*params_it).getValue()<<"'\n";
        incrementCount(&referer_params, (*params_it));
      }
    }
  }
  else if(hostname_type == 3 ) {
    for( params_it = params.begin(); params_it!=params.end(); params_it++){
      if((*params_it).getKey() == "q"){
        //std::cout<<"search_terms_param key '"<<(*params_it).getKey()<<"'\n";
        //std::cout<<"search_terms_param value '"<<(*params_it).getValue()<<"'\n";
        incrementCount(&search_queries, (*params_it).getValue());
      }
    }
  }
}

