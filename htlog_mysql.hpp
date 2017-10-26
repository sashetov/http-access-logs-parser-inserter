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
template<typename T> T getNthNode( std::map<T,int>, int);
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
    int getDomainsId( std::string );
    std::vector<std::string> getUserHostnames( int );
    int getUserId(int );
    void insertClientIps( std::map<unsigned long,int> &, std::map<unsigned long, int> );
    void insertDomains();
    void insertStringEntities( std::string, std::string, std::map<std::string,int> &, std::map<std::string, int> );
    void insertExternalDomains( std::map<std::string,int> &, std::map<std::string,int> );
    void insertNameVersionEntities(std::string, std::string, std::map<KeyValueContainer,int> &, std::map<KeyValueContainer,int> );
    void insertSearchTerms(std::map<KeyValueContainer,int> &, std::map<KeyValueContainer, int>, std::map<std::string,int>);
    void insertTrafficVectors(bool inner, std::map<TVectorContainer,int> &, std::map<TVectorContainer,int>, std::map<std::string,int>, std::map<std::string,int>);
    void insertHitsPerHour( std::map<HourlyHitsContainer,int>,int);
    void insertVisitsPerHour( std::map<HourlyVisitsContainer,int>, int, std::map<unsigned long, int>);
    void insertPageviewsPerHour( std::map<HourlyPageviewsContainer,int>, int, std::map<unsigned long, int>, std::map<std::string, int> );
    void insertLocationsPerHour( std::map<HourlyPageviewsContainer,int>, int, std::map<unsigned long, int>, std::map<std::string, int> );
    void insertBrowsersPerHour();
    void inserDevicestPerHour();
    void insertOsesPerHour();
    void insertBandwidthPerHour();
    void insertTrafficVectorsPerHour();
    void insertReferersPerHour( std::map<HourlyReferersContainer,int>, int, std::map<std::string,int>, std::map<std::string,int>);
    void insertSearchTermsPerHour( std::map<HourlySearchTermsContainer,int>, int, std::map<std::string,int>, std::map<KeyValueContainer,int>, std::map<std::string,int> );
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
};
#define __HTLOG_MYSQL__
#endif
