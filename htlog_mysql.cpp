#include "htlog_mysql.hpp"
LogsMysql::LogsMysql(std::string mysql_host, int mysql_port, std::string mysql_user, std::string mysql_password){
  host = mysql_host;
  port = mysql_port;
  username = mysql_user;
  password = mysql_password;
  mysql_url = "tcp://"+host+":"+std::to_string(port);
}
bool LogsMysql::prepare_execute(std::string database, std::string sql) {
  sql::Driver * driver = sql::mysql::get_driver_instance();
  boost::scoped_ptr< sql::Connection > conn(driver->connect(mysql_url, username, password));
  sql::PreparedStatement * prep_stmt = conn->prepareStatement(sql.c_str());;
  boost::scoped_ptr< sql::Statement > stmt(conn->createStatement());
  stmt->execute("USE " + database);
  prep_stmt->execute();
  delete prep_stmt;
  conn.reset();
  return true;
}
int LogsMysql::getDomainsId(  std::string domain_name ){
  int did=-1;
  std::string database = "cluster";
  std::string real_domain_name;
  try {
    sql::Driver * driver = sql::mysql::get_driver_instance();
    //std::cout<<"mysql_url: "<<mysql_url<<" username: "<<username<<" password: "<<password<<"\n";
    boost::scoped_ptr< sql::Connection > conn(driver->connect(mysql_url, username, password));
    conn->setSchema(database);
    boost::scoped_ptr< sql::Statement > stmt(conn->createStatement());
    std::string sql= "SELECT did FROM domains where domain_name ='"+ domain_name + "';";
    boost::scoped_ptr< sql::ResultSet > res(stmt->executeQuery(sql));
    while (res->next()) {
      did = std::stoi( res->getString("did"));
      break; // should only be one, break on first
    }
    sql = "SELECT domain_name FROM domains WHERE alias_of="+std::to_string(did)+" OR did="+std::to_string(did)+";";
    res.reset(stmt->executeQuery(sql));
    while (res->next()) {
      real_domain_name = res->getString("domain_name");
      break; // should only be one, break on first
    }
    sql= "SELECT did FROM domains where domain_name ='"+ real_domain_name + "';";
    res.reset(stmt->executeQuery(sql));
    while (res->next()) {
      did = std::stoi( res->getString("did"));
      break; // should only be one, break on first
    }
  } catch (sql::SQLException &e) {
    std::cout << "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
  return did;
}
int LogsMysql::getUserId( int real_did ){
  int uid=-1;
  std::string database = "cluster";
  try {
    sql::Driver * driver = sql::mysql::get_driver_instance();
    boost::scoped_ptr< sql::Connection > conn(driver->connect(mysql_url, username, password));
    conn->setSchema(database);
    boost::scoped_ptr< sql::Statement > stmt(conn->createStatement());
    std::string sql= "SELECT uid FROM domains where did ="+ std::to_string(real_did)+ ";";
    boost::scoped_ptr< sql::ResultSet > res(stmt->executeQuery(sql));
    while (res->next()) {
      uid = std::stoi( res->getString("uid"));
      break; // should only be one, break on first
    }
  } catch (sql::SQLException &e) {
    std::cout << "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
  return uid;
}
void LogsMysql::insertClientIps(std::map<unsigned long,int> &client_ips_ids, std::map<unsigned long, int> client_ips) {
  try {
    std::string database = "httpstats_clients";
    sql::Driver * driver = sql::mysql::get_driver_instance();
    boost::scoped_ptr< sql::Connection > conn(driver->connect(mysql_url, username, password));
    conn->setSchema(database);
    std::string insert_sql = "INSERT IGNORE INTO ips(ipv4) VALUES ";
    std::string select_ids_sql = "SELECT ipv4,id FROM ips WHERE ipv4 IN (";
    std::map<unsigned long,int>::iterator ul_it;
    for( ul_it = client_ips.begin(); ul_it!=client_ips.end(); ul_it++){
        unsigned long ip= ul_it->first;
        insert_sql += "("+std::to_string(ip)+"),";
        select_ids_sql+= std::to_string(ip)+",";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1); // chop off last ','
    insert_sql +=";";
    select_ids_sql = select_ids_sql.substr(0, select_ids_sql.size()-1); // chop off last ','
    select_ids_sql +=");";
    boost::scoped_ptr< sql::Statement > stmt(conn->createStatement());
    std::cout<<"insert_sql: "<<insert_sql<<"\n";
    //boost::scoped_ptr< sql::ResultSet > res(stmt->executeQuery(insert_sql));
    stmt->execute(insert_sql);
    std::cout<<"select_ids_sql: "<<select_ids_sql<<"\n";
    boost::scoped_ptr< sql::ResultSet > res(stmt->executeQuery(select_ids_sql));
    while (res->next()) {
      unsigned long ip = std::stol( res->getString("ipv4"));
      int id = std::stoi( res->getString("id"));
      client_ips_ids.insert({ip,id});
    }
  } catch (sql::SQLException &e) {
    std::cout << "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
LogsMysql::~LogsMysql(){
}
