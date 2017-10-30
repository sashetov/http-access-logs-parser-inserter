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
void set_id_safely( int val );
void spawn_when_ready( int tid, int tpool_size, int tmax, int &ncompleted );
void start_thread_pool( int tpool_size );
std::string getHostnameFromLogfile( std::string filename );
typedef struct  logline {
  std::string   hostname;
  unsigned long userIP;
  std::string   userIPStr;
  std::string   date;
  time_t        timestamp;
  std::string   requestPath;
  std::string   requestType;
  std::string   referer;
  std::string   agent;
  long          sizeBytes;
  int           statusCode;
} parsed_logline;
typedef struct url_parts {
  std::string   protocol;
  std::string   full_path;
  std::string   hostname;
  std::string   params;
} url_parts;
void print_url_parts( url_parts up );
class HttpAccessLogMetrics {
  public:
    HttpAccessLogMetrics( std::string user_host, std::vector<SearchEngineContainer> search_hosts, std::string file );
    ~HttpAccessLogMetrics();
    //HELPER METHODS
    unsigned long getDomainId();
    unsigned long getNumericIp( std::string addr );
    //DATA TANSORMATION FUNCTIONS
    std::string getStringIP( unsigned long ip );
    time_t getTimestamp( std::string dateTime );
    std::string getCountryFromIP( std::string client_ip);
    //PARSING FUNCTIONS
    std::vector<ParamsContainer> parseParamsString( std::string params_str, int type, std::string hostname, std::string page_path_full );
    int parseLine( std::string line, parsed_logline &ll );
    //GENERIC MAPPING INCREMENTOR
    template<typename T> void incrementCount( std::map<T,unsigned long> *kvmap, T key );
    template<typename T> void incrementCount( std::map<T,int> *kvmap, T key );
    //MAPPING FUNCTIONS
    int logsScan( );
    void processUserAgent( const UserAgent ua );
    void processTrafficVectors( std::string request, std::string referer, time_t timestamp );
    void processRequestUrlAndBandwidth( time_t timestamp, std::string request, unsigned long size_kb );
    void processHitsHourly( int real_did, time_t timestamp );
    void processVisitsHourly( int real_did, unsigned long user_ip, time_t timestamp );
    void processPageviewsHourly( int real_did, unsigned long user_ip, std::string request_str, time_t timestamp );
    void processSearchTermsHourly( std::string referer_str, time_t timestamp  );
    void processReferersHourly( std::string referer_str, time_t timestamp );
    void processUserAgentEntitiesHourly( const UserAgent ua, time_t timestamp );
    void processLocationsHourly( std::string location, time_t timestamp );
    //INSERTER FUNCTION
    void insertEntities( );
    Timer * timer;
  private:
    //LOADED SEARCH ENGINES PROPERTIES
    std::vector<SearchEngineContainer> search_engines;
    //USER ID, MAIN DOMAIN ID FROM DB
    unsigned long uid;
    unsigned long real_did;
    //LOGFILE PATH AND STATS
    int lines_failed;
    int lines_processed;
    std::string filename;
    //ALL THE USER'S DOMAIN ALIASES, STARTING WITH MAIN DOMAIN
    std::vector<std::string> internal_hostnames;
    //UNIQUE ENTITIES:COUNTS MAPPINGS
    std::map<unsigned long, int> client_ips;
    std::map<std::string,int> client_geo_locations;
    std::map<KeyValueContainer,int> client_devices;
    std::map<KeyValueContainer,int> client_oses;
    std::map<KeyValueContainer,int> client_browsers;
    std::map<std::string,int> page_paths_full;
    std::map<std::string,int> referer_hostnames;
    std::map<KeyValueContainer,int> search_queries;
    std::map<TVectorContainer,int> tvectors_inner;
    std::map<TVectorContainer,int> tvectors_incoming;
    //UNIQUE PER HOUR ENTITIES MAPPINGS
    std::map<HourlyHitsContainer,int> hits_per_hour;
    std::map<HourlyVisitsContainer,int> visits_per_hour;
    std::map<HourlyPageviewsContainer,int> pageviews_per_hour;
    std::map<HourlyLocationsContainer,int> locations_per_hour;
    std::map<HourlyUserAgentEntityContainer,int> devices_per_hour;
    std::map<HourlyUserAgentEntityContainer,int> browsers_per_hour;
    std::map<HourlyUserAgentEntityContainer,int> oses_per_hour;
    std::map<HourlyBandwidthContainer,int> bandwidth_per_hour;
    std::map<HourlyTVContainer,int> tvectors_inner_per_hour;
    std::map<HourlyTVContainer,int> tvectors_incoming_per_hour;
    std::map<HourlyReferersContainer,int> referers_per_hour;
    std::map<HourlySearchTermsContainer,int> search_terms_per_hour;
    //ENTITY TO ID MAPPINGS
    std::map<unsigned long, unsigned long> client_ips_ids;            // ip_id
    std::map<std::string,unsigned long> client_geo_locations_ids;     // country_id
    std::map<KeyValueContainer,unsigned long> client_devices_ids;     // device_id
    std::map<KeyValueContainer,unsigned long> client_oses_ids;        // os_id
    std::map<KeyValueContainer,unsigned long> client_browsers_ids;    // browser_id
    std::map<std::string,unsigned long> page_paths_full_ids;          // full_page_id
    std::map<std::string,unsigned long> referer_hostnames_ids;        // referer_domain_id
    std::map<KeyValueContainer,unsigned long> search_queries_ids;     // search_terms_ids
    std::map<TVectorContainer,unsigned long> tvectors_inner_ids;      // tvin_id
    std::map<TVectorContainer,unsigned long> tvectors_incoming_ids;   // tvinc_id
    //INTERFACE TO MYSQL STORAGE ROUTINES
    LogsMysql lm;
    //HELPERS FOR PARSING REQUEST AND REFERER URLS
    url_parts getUrlParts( std::string url_string, bool is_referer );
};
#define __HTLOG_PROCESSING__
#endif
