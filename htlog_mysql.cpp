#include "htlog_mysql.hpp"
ParamsContainer getNthPc( std::map<ParamsContainer,int> pcs, int n ) {
  int i =0;
  std::map<ParamsContainer,int>::iterator pc_it;
  for( pc_it = pcs.begin(); pc_it!=pcs.end(); pc_it++){
    ParamsContainer pC = pc_it->first;
    if( i == n ) {
      return pC;
    }
    i++;
  }
  return ParamsContainer(-1,"","","","",""); // return fake match with type -1 on fail to find
}
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
    std::cout<<"client ips insert_sql: "<<insert_sql<<"\n";
    //boost::scoped_ptr< sql::ResultSet > res(stmt->executeQuery(insert_sql));
    stmt->execute(insert_sql);
    std::cout<<"client ips select_ids_sql: "<<select_ids_sql<<"\n";
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
void LogsMysql::insertStringEntities(std::string database, std::string table, std::map<std::string,int> &entity_ids_map, std::map<std::string, int> entities ) {
  try {
    sql::Driver * driver = sql::mysql::get_driver_instance();
    boost::scoped_ptr< sql::Connection > conn(driver->connect(mysql_url, username, password));
    conn->setSchema(database);
    std::string insert_sql = "INSERT IGNORE INTO "+table+"(name) VALUES ";
    std::string select_ids_sql = "SELECT name,id FROM "+table+" WHERE name IN (";
    std::map<std::string,int>::iterator str_it;
    for( str_it = entities.begin(); str_it!=entities.end(); str_it++){
        std::string name = str_it->first;
        insert_sql += "('"+name+"'),";
        select_ids_sql+= "'"+name+"',";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1); // chop off last ','
    insert_sql +=";";
    select_ids_sql = select_ids_sql.substr(0, select_ids_sql.size()-1); // chop off last ','
    select_ids_sql +=");";
    boost::scoped_ptr< sql::Statement > stmt(conn->createStatement());
    std::cout<<database<<"."<<table<<" insert_sql: "<<insert_sql<<"\n";
    stmt->execute(insert_sql);
    std::cout<<database<<"."<<table<<"select_ids_sql: "<<select_ids_sql<<"\n";
    boost::scoped_ptr< sql::ResultSet > res(stmt->executeQuery(select_ids_sql));
    while (res->next()) {
      std::string name = res->getString("name");
      int id = std::stoi( res->getString("id"));
      entity_ids_map.insert({name,id});
    }
  } catch (sql::SQLException &e) {
    std::cout << "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertNameVersionEntities(std::string database, std::string table, std::map<KeyValueContainer,int> &entity_ids_map, std::map<KeyValueContainer, int> entities) {
  try {
    sql::Driver * driver = sql::mysql::get_driver_instance();
    boost::scoped_ptr< sql::Connection > conn(driver->connect(mysql_url, username, password));
    conn->setSchema(database);
    std::string insert_sql = "INSERT IGNORE INTO "+table+"(name,version) VALUES ";
    std::string select_ids_sql = "SELECT name,version,id FROM "+table+" WHERE ";
    std::map<KeyValueContainer,int>::iterator kv_it;
    for( kv_it = entities.begin(); kv_it!=entities.end(); kv_it++){
      KeyValueContainer kvC = kv_it->first;
      std::string name = kvC.getKey();
      std::string version= kvC.getValue();
      insert_sql += "('"+name+"','"+version+"'),";
      select_ids_sql+= "(name = '"+name+"' AND version = '"+version+"') OR ";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1); // chop off ','
    insert_sql +=";";
    select_ids_sql = select_ids_sql.substr(0, select_ids_sql.size()-3); // chop off last ','
    select_ids_sql +=";";
    boost::scoped_ptr< sql::Statement > stmt(conn->createStatement());
    std::cout<<database<<"."<<table<<" insert_sql: "<<insert_sql<<"\n";
    stmt->execute(insert_sql);
    std::cout<<database<<"."<<table<<"select_ids_sql: "<<select_ids_sql<<"\n";
    boost::scoped_ptr< sql::ResultSet > res(stmt->executeQuery(select_ids_sql));
    while (res->next()) {
      std::string name = res->getString("name");
      std::string version = res->getString("version");
      int id = std::stoi( res->getString("id"));
      KeyValueContainer nv = KeyValueContainer(name,version);
      entity_ids_map.insert({nv,id});
    }
  } catch (sql::SQLException &e) {
    std::cout << "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertParamsEntities(std::map<ParamsContainer,int> &entity_ids_map, std::map<ParamsContainer,int> entities, std::map<std::string,int> page_paths_ids, std::map<std::string,int> page_paths_full_ids) {
  try {
    std::string database = "httpstats_pages";
    std::string table = "url_params";
    sql::Driver * driver = sql::mysql::get_driver_instance();
    boost::scoped_ptr< sql::Connection > conn(driver->connect(mysql_url, username, password));
    conn->setSchema(database);
    std::string insert_sql = "INSERT IGNORE INTO "+table+"(page_id,full_page_id,k,val) VALUES ";
    std::string select_ids_sql = "SELECT id,k,val,full_page_id,page_id FROM "+table+" WHERE ";
    std::map<ParamsContainer,int>::iterator pc_it;
    for( pc_it = entities.begin(); pc_it!=entities.end(); pc_it++){
      ParamsContainer pC = pc_it->first;
      std::string path = pC.getPage();
      int path_id = page_paths_ids.find(path)->second;
      std::string full_path = pC.getFullPagePath();
      std::string key = pC.getKey();
      std::string value = pC.getValue();
      std::cout<<pC.toString()<<"\n";
      int full_path_id = page_paths_full_ids.find(full_path)->second;
      insert_sql += "("+std::to_string(path_id)+","+std::to_string(full_path_id)+",'"+key+"','"+value+"'),";
      select_ids_sql+= "(full_page_id = "+std::to_string(full_path_id)+" AND page_id = "+std::to_string(path_id)+" AND k = '"+key+"' AND val = '"+value+"' ) OR ";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1); // chop off ','
    insert_sql +=";";
    select_ids_sql = select_ids_sql.substr(0, select_ids_sql.size()-3); // chop off last ','
    select_ids_sql +=";";
    boost::scoped_ptr< sql::Statement > stmt(conn->createStatement());
    std::cout<<database<<"."<<table<<" insert_sql: "<<insert_sql<<"\n";
    stmt->execute(insert_sql);
    std::cout<<database<<"."<<table<<"select_ids_sql: "<<select_ids_sql<<"\n";
    boost::scoped_ptr< sql::ResultSet > res(stmt->executeQuery(select_ids_sql));
    int i =0;
    if(res->rowsCount() != entities.size()){
      throw std::runtime_error("rows count != entities number "+std::to_string(res->rowsCount())+"!="+std::to_string(entities.size()));
    }
    while (res->next()) {
      int id = std::stoi( res->getString("id"));
      ParamsContainer found_pc = getNthPc(entities,i);
      entity_ids_map.insert({found_pc,id});
      i++;
    }
  } catch (sql::SQLException &e) {
    std::cout << "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  } catch ( std::runtime_error &e) {
    std::cout << "# ERR: " << e.what() << "\n";
  }
}
void LogsMysql::insertExternalDomains(std::map<std::string,int> &referer_hostnames_ids, std::map<std::string, int> referer_hostnames) {
  try {
    std::string database = "httpstats_pages";
    std::string table = "external_domains";
    sql::Driver * driver = sql::mysql::get_driver_instance();
    boost::scoped_ptr< sql::Connection > conn(driver->connect(mysql_url, username, password));
    conn->setSchema(database);
    std::string insert_sql = "INSERT IGNORE INTO "+table+"(name) VALUES ";
    std::string select_ids_sql = "SELECT name,id FROM "+table+"WHERE name IN (";
    std::map<std::string,int>::iterator ul_it;
    for( ul_it = referer_hostnames.begin(); ul_it!=referer_hostnames.end(); ul_it++){
        std::string hostname = ul_it->first;
        insert_sql += "('"+hostname+"'),";
        select_ids_sql+= "'"+hostname+"',";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1); // chop off last ','
    insert_sql +=";";
    select_ids_sql = select_ids_sql.substr(0, select_ids_sql.size()-1); // chop off last ','
    select_ids_sql +=");";
    boost::scoped_ptr< sql::Statement > stmt(conn->createStatement());
    std::cout<<database<<"."<<table<<" insert_sql: "<<insert_sql<<"\n";
    stmt->execute(insert_sql);
    std::cout<<database<<"."<<table<<"select_ids_sql: "<<select_ids_sql<<"\n";
    boost::scoped_ptr< sql::ResultSet > res(stmt->executeQuery(select_ids_sql));
    while (res->next()) {
      std::string name = res->getString("name");
      int id = std::stoi( res->getString("id"));
      referer_hostnames_ids.insert({name,id});
    }
  } catch (sql::SQLException &e) {
    std::cout << "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
LogsMysql::~LogsMysql(){
}
