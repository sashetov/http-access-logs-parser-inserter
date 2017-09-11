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
  std::string  requestURL;
  std::string  requestType;
  std::string  referrer;
  std::string  agent;
  long sizeBytes;
  int statusCode;
} parsed_logline;
typedef struct url_parts {
  std::string protocol;
  std::string hostname;
  std::string path;
  std::string params;
}url_parts;
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
    void processRefererStrings( std::string );
    void processRequestUrl( std::string );
    std::string getCountryFromIP( std::string );
    std::vector<KeyValueContainer> parseParamsString( std::string );
    std::vector<ParamsContainer> parseParamsString( std::string, int, std::string, std::string );
    void insertClientIps( );
    std::map<unsigned long,int> getClientIps();
    void printClientIpsIpds();
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
    std::map<std::string,int> client_geo_locations;         // country_id
    std::map<std::string,int> client_devices;               // with client_devices vers device_id 
    std::map<std::string,int> client_oses;                  // os_id
    std::map<std::string,int> client_browsers;              // browser_id
    std::map<std::string,int> page_paths_full;              // full_page_id, needs domains_id
    std::map<std::string,int> referer_hostnames;            // referer_domain_id
    std::map<std::string,int> referer_pathstrings;          // with referer_hostnames: referer_id, referer_domain_id
    std::map<ParamsContainer,int> referer_params;           // with internref_hostnames, internref_pathstrings, internref_PARAMS: DOMAIN_id, page_id, url_param_id
    std::map<std::string,int> internref_hostnames;          // domain_id
    std::map<std::string,int> internref_pathstrings;        // page_id
    std::map<ParamsContainer,int> internref_params;         // url_param_id
    std::map<KeyValueContainer,int> search_queries;         // search_term_id
    std::map<std::string,int> tvectors_inner;               // needs page_id
    std::map<std::string,int> tvectors_inner_per_hour;      // needs page_id
    std::map<std::string,int> tvectors_incoming;            // needs referer_id
    std::map<std::string,int> tvectors_inc_per_hour;        // needs referer_id
    std::map<std::string,int> hits;                         // needs page_id
    std::map<std::string,int> visits;                       // needs ip_id entities
    std::map<std::string,int> pageviews;                    // needs page_id, ip_id
    //mysql
    LogsMysql lm;
    //methods
    int getLinesNumber();
    url_parts getUrlParts( std::string );
    url_parts getUrlPartsFromReqURL( std::string, std::string, std::string );
};
#define __HTLOG_PROCESSING__
#endif
