#include "htlog_mysql.hpp"
//EXTERN GLOBALS
extern std::string sql_logs_path;
//MISC UTILITY FUNCS
template<typename T> T getNthNode( std::map<T,int> pcs, int n ) {
  int i =0;
  typename std::map<T,int>::iterator it;
  for( it = pcs.begin(); it!=pcs.end(); it++){
    T node = it->first;
    if( i == n ) {
      return node;
    }
    i++;
  }
  return T();// return default version with empty strings..
}
std::string find_string_key_by_value( std::map<std::string, int> str_ids_map, int search_id ){
  int id;
  std::string str_val;
  std::map<std::string,int>::iterator fpit;
  for( fpit  = str_ids_map.begin(); fpit!=str_ids_map.end(); fpit++){
    str_val = fpit->first;
    id = fpit->second;
    if(id == search_id){
      break;
    }
  }
  return str_val;
}
//LogsMysql
LogsMysql::LogsMysql(std::string domain, std::string mysql_host, int mysql_port, std::string mysql_user, std::string mysql_password) : host(mysql_host), port(mysql_port), username(mysql_user), password(mysql_password), mysql_url("tcp://"+host+":"+std::to_string(port)), domain_name(domain) {
}
void LogsMysql::initThread(){
  driver = sql::mysql::get_driver_instance();
  handler = new st_worker_thread_param;
  handler->driver = driver;
  handler->driver->threadInit();
}
void LogsMysql::endThread(){
  handler->driver->threadEnd();
  delete(handler);
}
sql::ResultSet * LogsMysql::runSelectQuery(boost::scoped_ptr< sql::Statement > & stmt, std::string sql) {
  if(LOG_SQL_STMTS) {
    std::string sql_logs_path_full = sql_logs_path+"/"+domain_name+".sql";
    std::ofstream logfile(sql_logs_path_full, std::fstream::in | std::fstream::out | std::fstream::app );
    logfile<<sql<<std::endl;
    logfile.close();
  }
  sql::ResultSet * res(stmt->executeQuery(sql));
  return res;
}
void LogsMysql::runQuery(boost::scoped_ptr< sql::Statement > & stmt, std::string sql) {
  if(LOG_SQL_STMTS) {
    std::string sql_logs_path_full = sql_logs_path+"/"+domain_name+".sql";
    std::ofstream logfile(sql_logs_path_full, std::fstream::in | std::fstream::out | std::fstream::app );
    logfile<<sql<<std::endl;
    logfile.close();
  }
  try {
    stmt->execute(sql);
  } catch (sql::SQLException &e) {
    std::cerr<< "exception during insert query : " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )"<<std::endl<<" for SQL:"<<sql<<std::endl;
  }
}
int LogsMysql::getDomainsId(  std::string domain ){
  int possible_did=-1, real_did=-1;
  std::string database = "cluster";
  try {
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    sql::mysql::MySQL_Connection * mysql_conn = dynamic_cast<sql::mysql::MySQL_Connection*>(handler->con);
    std::string sql= "SELECT did FROM domains where domain_name ='"+ mysql_conn->escapeString(domain) + "' LIMIT 1;";
    boost::scoped_ptr< sql::ResultSet > res( runSelectQuery( stmt, sql ) );
    if (res->next()) {
      possible_did = std::stoi( res->getString("did"));
    }
    else {
      throw std::runtime_error("could not find a possible did for hostname: "+domain);
    }
    sql = "SELECT d1.did as real_did FROM domains d1 JOIN domains d2 on d1.did=d2.alias_of WHERE d1.alias_of=0 AND d1.did="+std::to_string(possible_did)+" OR d2.did="+std::to_string(possible_did)+" LIMIT 1;";
    res.reset(runSelectQuery(stmt,sql));
    if ( res->next() ) {
      real_did =  std::stoi(res->getString("real_did"));
    }
    else {
      throw std::runtime_error("could not find a real did for hostname: "+domain); 
    }
  }
  catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::getDomainsId caught sql exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
  catch (std::exception &e) {
    std::cerr<< "LogsMysql::getDomainsId caught exception: " << e.what() << " \n";
  }
  return real_did;
}
std::vector<std::string> LogsMysql::getUserHostnames( int real_did ){
  std::string database = "cluster";
  std::vector<std::string> internal_hostnames;
  try {
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    std::string all_domains_real_first_sql = "SELECT domain_name FROM domains WHERE alias_of="+ std::to_string(real_did)+ " OR did="+ std::to_string(real_did)+ " ORDER BY alias_of ASC;";
    boost::scoped_ptr< sql::ResultSet > res( runSelectQuery( stmt, all_domains_real_first_sql) );
    while (res->next()) {
      std::string domain = res->getString("domain_name");
      internal_hostnames.push_back(domain);
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::getDomainsId caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
  return internal_hostnames;
}
int LogsMysql::getUserId( int real_did ){
  int uid=-1;
  std::string database = "cluster";
  try {
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    sql::mysql::MySQL_Connection * mysql_conn = dynamic_cast<sql::mysql::MySQL_Connection*>(handler->con);
    std::string sql= "SELECT uid FROM domains where did ="+ mysql_conn->escapeString(std::to_string(real_did))+ ";";
    boost::scoped_ptr< sql::ResultSet > res(runSelectQuery(stmt,sql));
    while (res->next()) {
      uid = std::stoi( res->getString("uid"));
      break; // should only be one, break on first
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::getUserId caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
  return uid;
}
void LogsMysql::insertClientIps(std::map<unsigned long,int> &client_ips_ids, std::map<unsigned long, int> client_ips) {
  try {
    std::string database = "httpstats_clients";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    std::string insert_sql = "INSERT IGNORE INTO ips(ipv4) VALUES ";
    std::string select_ids_sql = "SELECT ipv4,id FROM ips WHERE ipv4 IN (";
    std::map<unsigned long,int>::iterator ul_it;
    sql::mysql::MySQL_Connection * mysql_conn = dynamic_cast<sql::mysql::MySQL_Connection*>(handler->con);
    for( ul_it = client_ips.begin(); ul_it!=client_ips.end(); ul_it++){
        unsigned long ip= ul_it->first;
        insert_sql += "("+mysql_conn->escapeString(std::to_string(ip))+"),";
        select_ids_sql+= mysql_conn->escapeString(std::to_string(ip))+",";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1); // chop off last ','
    insert_sql +=";";
    select_ids_sql = select_ids_sql.substr(0, select_ids_sql.size()-1); // chop off last ','
    select_ids_sql +=");";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if(client_ips.size() > 0){
      //std::cout<<"client ips insert_sql: "<<insert_sql<<"\n";
      runQuery(stmt,insert_sql);
      //std::cout<<"client ips select_ids_sql: "<<select_ids_sql<<"\n";
      boost::scoped_ptr< sql::ResultSet > res(runSelectQuery(stmt,select_ids_sql));
      while (res->next()) {
        unsigned long ip = std::stol( res->getString("ipv4"));
        int id = std::stoi( res->getString("id"));
        client_ips_ids.insert({ip,id});
      }
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertClientIps caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertStringEntities(std::string database, std::string table, std::map<std::string,int> &entity_ids_map, std::map<std::string, int> entities ) {
  try {
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    sql::mysql::MySQL_Connection * mysql_conn = dynamic_cast<sql::mysql::MySQL_Connection*>(handler->con);
    std::string insert_sql = "INSERT IGNORE INTO "+mysql_conn->escapeString(table)+"(name) VALUES ";
    std::string select_ids_sql = "SELECT id,name FROM "+mysql_conn->escapeString(table)+" WHERE name IN (";
    std::map<std::string,int>::iterator str_it;
    for( str_it = entities.begin(); str_it!=entities.end(); str_it++){
        std::string name = str_it->first;
        insert_sql += "('"+mysql_conn->escapeString(name)+"'),";
        select_ids_sql+= "'"+mysql_conn->escapeString(name)+"',";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1)+";";
    select_ids_sql = select_ids_sql.substr(0, select_ids_sql.size()-1)+");";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( entities.size() > 0){
      runQuery(stmt,insert_sql);
      boost::scoped_ptr< sql::ResultSet > res(runSelectQuery(stmt,select_ids_sql));
      while (res->next()) {
        int id = (int) std::stoi( res->getString("id"));
        std::string name = res->getString("name");
        entity_ids_map.insert({name,id});
      }
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertStringEntities caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertNameVersionEntities(std::string database, std::string table, std::map<KeyValueContainer,int> &entity_ids_map, std::map<KeyValueContainer, int> entities) {
  try {
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    sql::mysql::MySQL_Connection * mysql_conn = dynamic_cast<sql::mysql::MySQL_Connection*>(handler->con);
    std::string insert_sql = "INSERT IGNORE INTO "+mysql_conn->escapeString(table)+"(name,version) VALUES ";
    std::string select_ids_sql = "SELECT name,version,id FROM "+mysql_conn->escapeString(table)+" WHERE ";
    std::map<KeyValueContainer,int>::iterator kv_it;
    for( kv_it = entities.begin(); kv_it!=entities.end(); kv_it++){
      KeyValueContainer kvC = kv_it->first;
      std::string name = kvC.getKey();
      std::string version= kvC.getValue();
      insert_sql += "('"+mysql_conn->escapeString(name)+"','"+mysql_conn->escapeString(version)+"'),";
      select_ids_sql+= "(name = '"+mysql_conn->escapeString(name)+"' AND version = '"+mysql_conn->escapeString(version)+"') OR ";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1) +";";
    select_ids_sql = select_ids_sql.substr(0, select_ids_sql.size()-3) +";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if(entities.size() > 0){
      runQuery(stmt,insert_sql);
      boost::scoped_ptr< sql::ResultSet > res(runSelectQuery(stmt,select_ids_sql));
      while (res->next()) {
        std::string name = res->getString("name");
        std::string version = res->getString("version");
        int id = std::stoi( res->getString("id"));
        KeyValueContainer nv = KeyValueContainer(name,version);
        entity_ids_map.insert({nv,id});
      }
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertNameVersionEntities caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertSearchTerms(std::map<KeyValueContainer,int> &entity_ids_map, std::map<KeyValueContainer, int> entities, std::map<std::string,int> referer_hostnames_ids) {
  try {
    std::string database = "httpstats_pages";
    std::string table = "search_terms";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    sql::mysql::MySQL_Connection * mysql_conn = dynamic_cast<sql::mysql::MySQL_Connection*>(handler->con);
    std::string insert_sql = "INSERT IGNORE INTO "+mysql_conn->escapeString(table)+"(name,search_engine_id) VALUES ";
    std::string select_ids_sql = "SELECT name,search_engine_id,id FROM "+mysql_conn->escapeString(table)+" WHERE ";
    std::map<KeyValueContainer,int>::iterator kv_it;
    for( kv_it = entities.begin(); kv_it!=entities.end(); kv_it++){
      KeyValueContainer kvC = kv_it->first;
      std::string name = kvC.getKey();
      std::string search_hostname = kvC.getValue();
      int sid =  (referer_hostnames_ids.find(search_hostname))->second;
      insert_sql += "('"+mysql_conn->escapeString(name)+"',"+mysql_conn->escapeString(std::to_string(sid))+"),";
      select_ids_sql+= "(name = '"+mysql_conn->escapeString(name)+"' AND search_engine_id= "+mysql_conn->escapeString(std::to_string(sid))+") OR ";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1); // chop off ','
    insert_sql +=";";
    select_ids_sql = select_ids_sql.substr(0, select_ids_sql.size()-3); // chop off last ','
    select_ids_sql +=";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( entities.size() > 0 ) {
      runQuery(stmt,insert_sql);
      boost::scoped_ptr< sql::ResultSet > res(runSelectQuery(stmt,select_ids_sql));
      int i =0;
      while (res->next()) {
        int id = std::stoi( res->getString("id"));
        std::string search_term = res->getString("name");
        std::string search_engine_id = res->getString("search_engine_id");
        KeyValueContainer nv = KeyValueContainer(search_term, search_engine_id );
        entity_ids_map.insert({nv,id});
        i++;
      }
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertSearchTerms caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertExternalDomains(std::map<std::string,int> &referer_hostnames_ids, std::map<std::string, int> referer_hostnames) {
  try {
    std::string database = "httpstats_pages";
    std::string table = "external_domains";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    sql::mysql::MySQL_Connection * mysql_conn = dynamic_cast<sql::mysql::MySQL_Connection*>(handler->con);
    std::string insert_sql = "INSERT IGNORE INTO "+mysql_conn->escapeString(table)+"(name) VALUES ";
    std::string select_ids_sql = "SELECT name,id FROM "+mysql_conn->escapeString(table)+"WHERE name IN (";
    std::map<std::string,int>::iterator ul_it;
    for( ul_it = referer_hostnames.begin(); ul_it!=referer_hostnames.end(); ul_it++){
        std::string hostname = ul_it->first;
        insert_sql += "('"+mysql_conn->escapeString(hostname)+"'),";
        select_ids_sql+= "'"+mysql_conn->escapeString(hostname)+"',";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1)+";";
    select_ids_sql = select_ids_sql.substr(0, select_ids_sql.size()-1)+");";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( referer_hostnames.size() > 0 ){
      runQuery(stmt,insert_sql);
      boost::scoped_ptr< sql::ResultSet > res(runSelectQuery(stmt,select_ids_sql));
      while (res->next()) {
        int id = (int)std::stoi( res->getString("id"));
        std::string name = res->getString("name");
        referer_hostnames_ids.insert({name,id});
      }
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertExternalDomains caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertTrafficVectors(bool inner, std::map<TVectorContainer,int> &tvectors_ids, std::map<TVectorContainer,int> tvectors, std::map<std::string,int> referer_hostnames_ids, std::map<std::string,int> page_paths_full_ids){
  try {
    std::string database = "httpstats_pages";
    std::string table = inner ? "tvectors_inn" : "tvectors_inc";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    std::chrono::steady_clock::time_point tp = std::chrono::steady_clock::now();
    long long timestamp = (long long) (std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch())).count();
    std::string temp_table_name = "ttab_for_join_"+std::to_string(timestamp);
    std::string columns_full =  inner ?  table+".a_id,"+table+".b_id" : table+".referer_domain_id,"+table+".referer_page_id,"+table+".page_id";
    std::string columns=  inner ?  "a_id,b_id" : "referer_domain_id,referer_page_id,page_id";
    std::string insert_sql ="INSERT IGNORE INTO "+table+"("+columns+") VALUES ";
    std::string create_temp_table_sql = "CREATE TEMPORARY TABLE "+temp_table_name+" like httpstats_pages."+table+";";
    std::string temp_table_insert_sql = "INSERT IGNORE INTO "+temp_table_name+"("+columns+") VALUES ";
    std::string join_on_clause = inner ? 
      "t.a_id="+table+".a_id AND t.b_id = "+table+".b_id " : 
      "t.referer_domain_id = "+table+".referer_domain_id AND t.referer_page_id = "+table+".referer_page_id AND t.page_id = "+table+".page_id ";
    std::string select_ids_sql = "SELECT "+table+".id,"+columns_full+" FROM "+table+" JOIN "+temp_table_name+" t ON "+join_on_clause+";";
    std::string insert_values="";
    std::map<TVectorContainer,int>::iterator tvc_it;
    for( tvc_it = tvectors.begin(); tvc_it!=tvectors.end(); tvc_it++){
      TVectorContainer tvC = tvc_it->first;
      std::string path_a = tvC.getPagePathA();
      std::string path_b = tvC.getPagePathB();
      int path_a_id = page_paths_full_ids.find(path_a)->second;
      int path_b_id = page_paths_full_ids.find(path_b)->second;
      if(inner){
        insert_values += "("+std::to_string(path_a_id)+","+std::to_string(path_b_id)+"),";
      }
      else {
        std::string external_domain = tvC.getExternalDomain();
        int referer_domain_id = referer_hostnames_ids.find(external_domain)->second;
        insert_values += "("+std::to_string(referer_domain_id)+","+std::to_string(path_a_id)+","+std::to_string(path_b_id)+"),";
      }
    }
    insert_values= insert_values.substr(0, insert_values.size()-1)+";";
    insert_sql += insert_values;
    temp_table_insert_sql += insert_values;
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    boost::scoped_ptr< sql::Statement > create_stmt(handler->con->createStatement());
    if( tvectors.size() ) {
      runQuery(stmt,insert_sql);
      runQuery(stmt,create_temp_table_sql);
      runQuery(stmt,temp_table_insert_sql);
      boost::scoped_ptr< sql::ResultSet > res(runSelectQuery(stmt,select_ids_sql));
      while ( res->next() ) {
        int col_index = 1;
        int id = std::stoi( res->getString(col_index));
        int external_domain_id = 0;
        if(!inner) {
          col_index = 2;
          external_domain_id = std::stoi(res->getString(col_index));
        }
        col_index = inner ? 2:3;
        int a_id = std::stoi( res->getString(col_index));
        col_index = inner ? 3:4;
        int b_id = std::stoi( res->getString(col_index));
        std::string page_path_a = find_string_key_by_value(page_paths_full_ids, a_id);
        std::string page_path_b = find_string_key_by_value(page_paths_full_ids, b_id);
        std::string external_domain = inner ? "" : find_string_key_by_value(referer_hostnames_ids, external_domain_id);
        TVectorContainer searchTvector(inner, external_domain, page_path_a, page_path_b);
        tvectors_ids.insert({searchTvector,id});
      }
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertTrafficVectors caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertHitsPerHour(std::map<HourlyHitsContainer,int> hits, int real_did){
  try {
    std::string database = "httpstats_domains";
    std::string table = "hits_hourly";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    sql::mysql::MySQL_Connection * mysql_conn = dynamic_cast<sql::mysql::MySQL_Connection*>(handler->con);
    std::string insert_sql = "INSERT IGNORE INTO "+mysql_conn->escapeString(table)+"(date,domains_id,count) VALUES ";
    std::map<HourlyHitsContainer,int>::iterator hhc_it;
    for( hhc_it = hits.begin(); hhc_it!=hits.end(); hhc_it++){
      HourlyHitsContainer hhC = hhc_it->first;
      int count = hhc_it->second;
      insert_sql += "('"+mysql_conn->escapeString(hhC.getTsMysql())+"',"+mysql_conn->escapeString(std::to_string(real_did))+","+mysql_conn->escapeString(std::to_string(count))+"),";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1)+";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( hits.size()>0 ){
      runQuery(stmt,insert_sql);
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertHitsPerHour caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertVisitsPerHour( std::map<HourlyVisitsContainer,int> visits, int real_did, std::map<unsigned long, int> client_ips_ids ){
  try {
    std::string database = "httpstats_clients";
    std::string table = "visits_hourly";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    sql::mysql::MySQL_Connection * mysql_conn = dynamic_cast<sql::mysql::MySQL_Connection*>(handler->con);
    std::string insert_sql = "INSERT IGNORE INTO "+mysql_conn->escapeString(table)+"(date,domains_id,ip_id,count) VALUES ";
    std::map<HourlyVisitsContainer,int>::iterator hvc_it;
    for( hvc_it = visits.begin(); hvc_it!=visits.end(); hvc_it++){
      HourlyVisitsContainer hvC = hvc_it->first;
      unsigned long ip = hvC.getIp();
      int count = hvc_it->second;
      int ip_id =  (client_ips_ids.find(ip))->second;
      insert_sql += "('"+mysql_conn->escapeString(hvC.getTsMysql())+"',"+mysql_conn->escapeString(std::to_string(real_did))+","+mysql_conn->escapeString(std::to_string(ip_id))+","+mysql_conn->escapeString(std::to_string(count))+"),";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1) +";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( visits.size() > 0 ) {
      runQuery(stmt,insert_sql);
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertVisitsPerHour caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertPageviewsPerHour( std::map<HourlyPageviewsContainer,int> pageviews, int real_did, std::map<unsigned long, int> client_ips_ids, std::map<std::string,int> page_paths_full_ids){
  try {
    std::string database = "httpstats_pages";
    std::string table = "pageviews_hourly";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    sql::mysql::MySQL_Connection * mysql_conn = dynamic_cast<sql::mysql::MySQL_Connection*>(handler->con);
    std::string insert_sql = "INSERT IGNORE INTO "+mysql_conn->escapeString(table)+"(date,domains_id,ip_id,page_id,count) VALUES ";
    std::map<HourlyPageviewsContainer,int>::iterator hpc_it;
    for( hpc_it = pageviews.begin(); hpc_it!=pageviews.end(); hpc_it++){
      HourlyPageviewsContainer hpC = hpc_it->first;
      unsigned long ip = hpC.getIp();
      std::string page_path = hpC.getPagePath();
      int count = hpc_it->second;
      int ip_id =  (client_ips_ids.find(ip))->second;
      int page_id =  (page_paths_full_ids.find(page_path))->second;
      insert_sql += "('"+mysql_conn->escapeString(hpC.getTsMysql())+"',"+mysql_conn->escapeString(std::to_string(real_did))+","+mysql_conn->escapeString(std::to_string(ip_id))+","+mysql_conn->escapeString(std::to_string(page_id))+","+mysql_conn->escapeString(std::to_string(count))+"),";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1)+ ";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( pageviews.size()> 0){
      runQuery(stmt,insert_sql);
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertPageviewsPerHour caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertReferersPerHour( std::map<HourlyReferersContainer,int> referers, int real_did, std::map<std::string,int> page_paths_full_ids, std::map<std::string,int> referer_hostnames_ids ){
  std::string database = "httpstats_pages";
  std::string table = "referers_hourly";
  try {
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    sql::mysql::MySQL_Connection * mysql_conn = dynamic_cast<sql::mysql::MySQL_Connection*>(handler->con);
    std::string insert_sql = "INSERT IGNORE INTO "+table+"(date,domains_id,page_id,referer_domain_id,count) VALUES ";
    std::map<HourlyReferersContainer,int>::iterator hrc_it;
    for( hrc_it = referers.begin(); hrc_it!=referers.end(); hrc_it++){
      HourlyReferersContainer hrC = hrc_it->first;
      std::string page_path = hrC.getRefererPathFull();
      std::string referer_domain = hrC.getRefererDomain();
      int page_id = page_paths_full_ids.find(page_path)->second;
      bool inner = hrC.isInnerReferer();
      int referer_domain_id = inner ? 0 : referer_hostnames_ids.find(referer_domain)->second;
      int count = hrc_it->second;
      insert_sql += "('"+ mysql_conn->escapeString(hrC.getTsMysql())+"',"+ 
        mysql_conn->escapeString(std::to_string(real_did))+","+ 
        mysql_conn->escapeString(std::to_string(page_id))+","+
        mysql_conn->escapeString(std::to_string(referer_domain_id))+","+
        mysql_conn->escapeString(std::to_string(count))+"),";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1)+";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( referers.size()> 0){
      runQuery(stmt,insert_sql);
    }
  }
  catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertPageviewsPerHour caught sql exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertSearchTermsPerHour( std::map<HourlySearchTermsContainer,int> search_terms, int real_did, std::map<std::string,int> page_paths_full_ids, std::map<KeyValueContainer,int> search_terms_ids, std::map<std::string,int> referer_hostnames_ids ){
  try {
    std::string database = "httpstats_pages";
    std::string table = "search_terms_hourly";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    sql::mysql::MySQL_Connection * mysql_conn = dynamic_cast<sql::mysql::MySQL_Connection*>(handler->con);
    std::string insert_sql = "INSERT IGNORE INTO "+table+"(date,domains_id,page_id,search_term_id,count) VALUES ";
    std::map<HourlySearchTermsContainer,int>::iterator hsc_it;
    std::map<KeyValueContainer,int>::iterator kvc_it;
    for( hsc_it = search_terms.begin(); hsc_it!=search_terms.end(); hsc_it++){
      HourlySearchTermsContainer hsC = hsc_it->first;
      std::string page_path = hsC.getPagePathFull();
      std::string search_term = hsC.getSearchTerm();
      std::string search_engine_hostname = hsC.getSearchEngineDomain();
      int sid =  (referer_hostnames_ids.find(search_engine_hostname))->second;
      KeyValueContainer stc = KeyValueContainer(search_term, std::to_string(sid));
      int page_id =  (page_paths_full_ids.find(page_path))->second;
      int search_term_id = (search_terms_ids.find(stc))->second;
      int count = hsc_it->second;
      insert_sql += "('"+ 
        mysql_conn->escapeString(hsC.getTsMysql())+"',"+ 
        mysql_conn->escapeString(std::to_string(real_did))+","+ 
        mysql_conn->escapeString(std::to_string(page_id))+","+ 
        mysql_conn->escapeString(std::to_string(search_term_id))+","+ 
        mysql_conn->escapeString(std::to_string(count))+"),";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1)+";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( search_terms.size()> 0){
      runQuery(stmt,insert_sql);
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertPageviewsPerHour caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
LogsMysql::~LogsMysql(){
}
