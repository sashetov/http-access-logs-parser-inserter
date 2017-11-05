#ifndef __HTLOG_MYSQL__
#include <string>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <driver/mysql_public_iface.h>
#include <cppconn/prepared_statement.h>
#include <boost/scoped_ptr.hpp>
#include <chrono>
#include <string.h>
#include <time.h>
#include "htlog_containers.hpp"
#define LOG_SQL_STMTS 1
std::string find_string_key_by_value( std::map<std::string, int>, int );
class LogsMysql {
  public:
    LogsMysql(std::string domain, std::string mysql_host, int mysql_port, std::string mysql_user, std::string mysql_password);
    ~LogsMysql();
    sql::Connection *con;
    std::string getTsMysql( time_t ts );
    unsigned long getDomainsId(  std::string domain );
    unsigned long getUserId( unsigned long real_did );
    sql::ResultSet * runSelectQuery(boost::scoped_ptr< sql::Statement > & stmt, std::string sql) ;
    void runQuery( boost::scoped_ptr< sql::Statement > & stmt, std::string sql) ;
    std::vector<std::string> getUserHostnames( unsigned long real_did );
    void insertClientIps( std::map<unsigned long,unsigned long> &client_ips_ids, std::map<unsigned long, int> client_ips) ;
    void insertStringEntities( std::string database, std::string table, std::map<std::string,unsigned long> &entity_ids_map, std::map<std::string, int> entities ) ;
    void insertExternalDomains( std::map<std::string,unsigned long> &referer_hostnames_ids, std::map<std::string, int> referer_hostnames) ;
    void insertNameVersionEntities( std::string database, std::string table, std::map<KeyValueContainer,unsigned long> &entity_ids_map, std::map<KeyValueContainer, int> entities);
    void insertSearchTerms( std::map<KeyValueContainer,unsigned long> &search_terms_ids, std::map<KeyValueContainer, int> search_terms, std::map<std::string,unsigned long> referer_hostnames_ids );
    void insertTrafficVectors(bool inner, std::map<TVectorContainer,unsigned long> &tvectors_ids, std::map<TVectorContainer,int> tvectors, std::map<std::string,unsigned long> referer_hostnames_ids, std::map<std::string,unsigned long> page_paths_full_ids, std::string domain_name);
    void insertHitsPerHour( std::map<HourlyHitsContainer,int> hits, unsigned long real_did) ;
    void insertVisitsPerHour( std::map<HourlyVisitsContainer,int> visits, unsigned long real_did, std::map<unsigned long, unsigned long> client_ips_ids );
    void insertPageviewsPerHour( std::map<HourlyPageviewsContainer,int> pageviews, unsigned long real_did, std::map<unsigned long, unsigned long> client_ips_ids, std::map<std::string,unsigned long> page_paths_full_ids);
    void insertReferersPerHour( std::map<HourlyReferersContainer,int> referers, unsigned long real_did, std::map<std::string,unsigned long> page_paths_full_ids, std::map<std::string,unsigned long> referer_hostnames_ids );
    void insertUserAgentEntitiesPerHour( std::map<HourlyUserAgentEntityContainer,int> devices_per_hour, std::map<HourlyUserAgentEntityContainer,int> oses_per_hour, std::map<HourlyUserAgentEntityContainer,int> browsers_per_hour, unsigned long real_did, std::map<KeyValueContainer,unsigned long> client_devices_ids, std::map<KeyValueContainer,unsigned long> client_oses_ids, std::map<KeyValueContainer,unsigned long> client_browsers_ids );
    void insertBandwidthPerHour( std::map<HourlyBandwidthContainer,int> bandwidth, unsigned long real_did, std::map<std::string,unsigned long> page_paths_full_ids);
    void insertTVCPerHour( bool is_inner, std::map<HourlyTVContainer,int> tvectors, unsigned long real_did, std::map<TVectorContainer,unsigned long> tvectors_ids );
    void insertLocationsPerHour( std::map<HourlyLocationsContainer,int> locations, unsigned long real_did, std::map<std::string,unsigned long> locations_ids );
    void insertSearchTermsPerHour( std::map<HourlySearchTermsContainer,int> search_terms, unsigned long real_did, std::map<std::string,unsigned long> page_paths_full_ids, std::map<KeyValueContainer,unsigned long> search_terms_ids, std::map<std::string,unsigned long> referer_hostnames_ids );
    void insertAllPerDay( unsigned long real_did, time_t ts  );
  private:
    std::string host;
    int port;
    std::string username;
    std::string password;
    std::string mysql_url;
    std::string domain_name;
    void buildAndRunHourlyUAEQuery(std::string aeph_table, std::string entity_id_name, std::map<HourlyUserAgentEntityContainer,int> uae_ph, unsigned long real_did, std::map<KeyValueContainer, unsigned long> user_agent_entity_ids );
    time_t roundTsToDay( time_t ts_full );
    time_t getTomorrowMidnight( time_t ts_full);
};
#define __HTLOG_MYSQL__
#endif
