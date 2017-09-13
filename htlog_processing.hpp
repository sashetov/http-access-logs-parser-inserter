#ifndef __HTLOG_PROCESSING__
#include <map>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <thread>
#include <fstream>
#include <arpa/inet.h>
#include <iostream>
#include <GeoLite2PP.hpp>
#include <string>
#include "htlog_uap.hpp"
#include "htlog_containers.hpp"
#include "htlog_mysql.hpp"
#define MYSQL_HOSTNAME "185.52.26.79"
#define MYSQL_PORT 3308
#define MYSQL_USER "root"
#define MYSQL_PASSWORD "qqq"
typedef struct logline {
  std::string  hostname;
  unsigned long  userIP;
  std::string userIPStr;
  std::string  date;
  unsigned long timestamp;
  std::string  requestPath;
  std::string  requestType;
  std::string  referer;
  std::string  agent;
  long sizeBytes;
  int statusCode;
} parsed_logline;
typedef struct url_parts {
  std::string protocol;
  std::string full_path;
  std::string hostname;
  std::string path;
  std::string params;
} url_parts;
class HttpAccessLogMetrics {
  public:
    HttpAccessLogMetrics( std::vector<std::string>, std::vector<std::string>,std::string);
    ~HttpAccessLogMetrics();
    int logsScanParallel(int,int,long);
    void parseLogFile(int);
    int parseLine(std::string,parsed_logline &);
    unsigned long getNumericIp( std::string );
    std::string getStringIP(unsigned long);
    unsigned long getTimestamp( std::string );
    template<typename T> void incrementCount( std::map<T,int>*, T );
    void processUserAgent( std::string );
    void processTrafficVectors( std::string, std::string );
    void processRequestUrl( std::string );
    std::string getCountryFromIP( std::string );
    std::vector<KeyValueContainer> parseParamsString( std::string );
    std::vector<ParamsContainer> parseParamsString( std::string, int, std::string, std::string, std::string );
    void insertEntities( );
    std::map<unsigned long,int> getClientIps();
    void printAllIdsMaps();
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
    std::map<unsigned long, int> client_ips;
    std::map<unsigned long, int> client_ips_ids;            // ip_id
    std::map<std::string,int> client_geo_locations;
    std::map<std::string,int> client_geo_locations_ids;     // country_id
    std::map<KeyValueContainer,int> client_devices;
    std::map<KeyValueContainer,int> client_devices_ids;     // with client_devices vers device_id 
    std::map<KeyValueContainer,int> client_oses;
    std::map<KeyValueContainer,int> client_oses_ids;        // os_id
    std::map<KeyValueContainer,int> client_browsers;
    std::map<KeyValueContainer,int> client_browsers_ids;    // browser_id
    std::map<std::string,int> page_paths_full;
    std::map<std::string,int> page_paths_full_ids;          // full_page_id
    std::map<std::string,int> referer_hostnames;
    std::map<std::string, int> referer_hostnames_ids;       // referer_domain_id
    std::map<std::string,int> referer_paths;
    std::map<std::string,int> referer_paths_ids;            // page_id
    std::map<ParamsContainer,int> referer_params;
    std::map<ParamsContainer,int> referer_params_ids;       // url_param_id
    std::map<std::string,int> internal_domains;             // domain_id ( imported, not inserted )
    std::map<std::string,int> internal_paths;
    std::map<std::string,int> internal_paths_ids;           // page_id
    std::map<ParamsContainer,int> internal_params;
    std::map<ParamsContainer,int> internal_params_ids;      // url_param_id
    std::map<KeyValueContainer,int> search_queries;
    std::map<KeyValueContainer,int> search_queries_ids;     // search_terms_ids
    std::map<TVectorContainer,int> tvectors_inner;
    std::map<TVectorContainer,int> tvectors_inner_ids;      // tvin_id
    std::map<TVectorContainer,int> tvectors_incoming;
    std::map<TVectorContainer,int> tvectors_incoming_ids;   // tvinc_id
    std::map<std::string,int> hits;
    std::map<std::string,int> visits;
    std::map<std::string,int> pageviews;
    //mysql
    LogsMysql lm;
    //methods
    int getLinesNumber();
    url_parts getUrlParts( std::string );
    url_parts getUrlPartsFromReqPath( std::string, std::string, std::string );
};
#define __HTLOG_PROCESSING__
#endif
