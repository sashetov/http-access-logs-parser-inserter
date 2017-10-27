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
#include "htlog_containers.hpp"
#define LOG_SQL_STMTS 1
std::string find_string_key_by_value( std::map<std::string, int>, int );
struct st_worker_thread_param {
  sql::Driver *driver;
  sql::Connection *con;
};
class LogsMysql {
  public:
    LogsMysql(std::string,std::string,int,std::string,std::string);
    void initThread();
    void endThread();
    sql::ResultSet * runSelectQuery(boost::scoped_ptr< sql::Statement > &, std::string );
    void runQuery(boost::scoped_ptr< sql::Statement > &, std::string);
    unsigned long getDomainsId( std::string );
    std::vector<std::string> getUserHostnames( unsigned long );
    unsigned long getUserId( unsigned long );
    void insertClientIps( std::map<unsigned long,unsigned long> &, std::map<unsigned long, int> );
    void insertDomains();
    void insertStringEntities( std::string, std::string, std::map<std::string,unsigned long> &, std::map<std::string, int> );
    void insertExternalDomains( std::map<std::string,unsigned long> &, std::map<std::string,int> );
    void insertNameVersionEntities(std::string, std::string, std::map<KeyValueContainer,unsigned long> &, std::map<KeyValueContainer,int> );
    void insertSearchTerms(std::map<KeyValueContainer,unsigned long> &, std::map<KeyValueContainer, int>, std::map<std::string,unsigned long>, std::string);
    void insertTrafficVectors(bool inner, std::map<TVectorContainer,unsigned long> &, std::map<TVectorContainer,int>, std::map<std::string,unsigned long>, std::map<std::string,unsigned long>, std::string );
    void insertHitsPerHour( std::map<HourlyHitsContainer,int>,unsigned long);
    void insertVisitsPerHour( std::map<HourlyVisitsContainer,int>,unsigned long, std::map<unsigned long, unsigned long>, std::string );
    void insertPageviewsPerHour( std::map<HourlyPageviewsContainer,int>,unsigned long, std::map<unsigned long, unsigned long>, std::map<std::string, unsigned long> );
    void insertLocationsPerHour( std::map<HourlyLocationsContainer,int>,unsigned long, std::map<std::string,unsigned long> );
    void insertUserAgentEntitiesPerHour( std::map<HourlyUserAgentEntityContainer,int>, std::map<HourlyUserAgentEntityContainer,int>, std::map<HourlyUserAgentEntityContainer,int>,unsigned long, std::map<KeyValueContainer,unsigned long>, std::map<KeyValueContainer,unsigned long>, std::map<KeyValueContainer,unsigned long>);
    void insertBandwidthPerHour( std::map<HourlyBandwidthContainer,int>,unsigned long, std::map<std::string,unsigned long> );
    void insertTVCPerHour( bool , std::map<HourlyTVContainer,int> , unsigned long, std::map<TVectorContainer,unsigned long> );
    void insertLocationsPerHour( bool, std::map<HourlyTVContainer,int>,unsigned long, std::map<TVectorContainer,unsigned long> );
    void insertReferersPerHour( std::map<HourlyReferersContainer,int>,unsigned long, std::map<std::string,unsigned long>, std::map<std::string,unsigned long>);
    void insertSearchTermsPerHour( std::map<HourlySearchTermsContainer,int>, unsigned long, std::map<std::string,unsigned long>, std::map<KeyValueContainer,unsigned long>, std::map<std::string,unsigned long> );
    ~LogsMysql();
  private:
    std::string host;
    int port;
    std::string username;
    std::string password;
    std::string mysql_url;
    std::string domain_name;
    sql::Driver * driver;
    struct st_worker_thread_param * handler;
    void buildAndRunHourlyUAEQuery(std::string, std::string, std::map<HourlyUserAgentEntityContainer,int>, unsigned long, std::map<KeyValueContainer,unsigned long>);
};
#define __HTLOG_MYSQL__
#endif
