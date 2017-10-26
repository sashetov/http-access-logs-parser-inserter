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
#include <errno.h>
#include <condition_variable>
#include <chrono>
#include "htlog_uap.hpp"
#include "htlog_analyzer.hpp"
#include "htlog_containers.hpp"
#include "htlog_mysql.hpp"
#include "htlog_timer.hpp"
std::string getHostnameFromLogfile(std::string);
typedef struct logline {
  std::string  hostname;
  unsigned long  userIP;
  std::string userIPStr;
  std::string  date;
  time_t timestamp;
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
    HttpAccessLogMetrics( std::string, std::vector<SearchEngineContainer>, std::string );
    ~HttpAccessLogMetrics();
    int getDomainId();
    int logsScan( );
    int parseLine(std::string,parsed_logline &);
    unsigned long getNumericIp( std::string );
    std::string getStringIP(unsigned long);
    time_t getTimestamp( std::string );
    template<typename T> void incrementCount( std::map<T,int>*, T );
    void processUserAgent( std::string );
    void processTrafficVectors( std::string, std::string );
    void processRequestUrl( std::string );
    std::vector<ParamsContainer> parseParamsString( std::string, int, std::string, std::string, std::string );
    void processSearchTerms( std::string, time_t );
    void processReferers( std::string, time_t );
    std::string getCountryFromIP( std::string );
    void insertEntities( );
    std::map<unsigned long,int> getClientIps();
    Timer * timer;
  private:
    LogsMysql lm;
    int real_did;
    int uid;
    int lines_failed;
    int lines_processed;
    std::string filename;
    std::vector<std::string> internal_hostnames;
    std::vector<SearchEngineContainer> search_engines;
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
    std::map<std::string,int> referer_hostnames_ids;        // referer_domain_id
    std::map<std::string,int> referer_paths;
    std::map<std::string,int> referer_paths_ids;            // page_id
    std::map<std::string,int> internal_domains;             // domain_id ( imported, not inserted )
    std::map<std::string,int> internal_paths;
    std::map<std::string,int> internal_paths_ids;           // page_id
    std::map<KeyValueContainer,int> search_queries;
    std::map<KeyValueContainer,int> search_queries_ids;     // search_terms_ids
    std::map<TVectorContainer,int> tvectors_inner;
    std::map<TVectorContainer,int> tvectors_inner_ids;      // tvin_id
    std::map<TVectorContainer,int> tvectors_incoming;
    std::map<TVectorContainer,int> tvectors_incoming_ids;   // tvinc_id
    std::map<HourlyHitsContainer,int> hits;
    std::map<HourlyVisitsContainer,int> visits;
    std::map<HourlyPageviewsContainer,int> pageviews;
    std::map<HourlyReferersContainer,int> referers;
    std::map<HourlySearchTermsContainer,int> search_terms;
    //methods
    int getLinesNumber();
    url_parts getUrlParts( std::string );
    url_parts getUrlPartsFromReqPath( std::string, std::string, std::string );
};
void set_id_safely( int );
void notify( int );
void spawn_when_ready( int, int, int, int& );
void start_thread_pool( int );
#define __HTLOG_PROCESSING__
#endif
