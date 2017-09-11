#ifndef __HTLOG_MYSQL__
#include <string>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <driver/mysql_public_iface.h>
#include <cppconn/prepared_statement.h>
#include <boost/scoped_ptr.hpp>
class LogsMysql {
  public:
    LogsMysql(std::string,int,std::string,std::string);
    bool prepare_execute( std::string, std::string );
    int getDomainsId( std::string );
    int getUserId(int );
    void insertClientIps( std::map<unsigned long,int> &, std::map<unsigned long, int> );
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
