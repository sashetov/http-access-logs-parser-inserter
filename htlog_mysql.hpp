#ifndef __HTLOG_MYSQL__
#include <string>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <driver/mysql_public_iface.h>
#include <cppconn/prepared_statement.h>
#include <boost/scoped_ptr.hpp>
#include "htlog_containers.hpp"
template<typename T> T getNthNode( std::map<T,int>, int);
class LogsMysql {
  public:
    LogsMysql(std::string,int,std::string,std::string);
    bool prepare_execute( std::string, std::string );
    int getDomainsId( std::string );
    int getUserId(int );
    void insertClientIps( std::map<unsigned long,int> &, std::map<unsigned long, int> );
    void insertDomains();
    void insertStringEntities( std::string, std::string, std::map<std::string,int> &, std::map<std::string, int> );
    void insertExternalDomains( std::map<std::string,int> &, std::map<std::string,int> );
    void insertNameVersionEntities(std::string, std::string, std::map<KeyValueContainer,int> &, std::map<KeyValueContainer,int> );
    void insertSearchTerms(std::map<KeyValueContainer,int> &, std::map<KeyValueContainer, int>, std::map<std::string,int>);
    void insertParamsEntities(std::map<ParamsContainer,int> &, std::map<ParamsContainer,int>, std::map<std::string,int>, std::map<std::string,int>);
    void insertTrafficVectors(bool inner, std::map<TVectorContainer,int> &, std::map<TVectorContainer,int>, std::map<std::string,int>, std::map<std::string,int>);
    ~LogsMysql();
  private:
    std::string host;
    int port;
    std::string username;
    std::string password;
    std::string mysql_url;
};
#define __HTLOG_MYSQL__
#endif
