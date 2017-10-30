#include "htlog_mysql.hpp"
extern unsigned long global_id;
extern std::string sql_logs_path;
std::string find_string_key_by_value( std::map<std::string, unsigned long> str_ids_map, unsigned long search_id ){
  unsigned long id;
  std::string str_val;
  std::map<std::string,unsigned long>::iterator fpit;
  for( fpit  = str_ids_map.begin(); fpit!=str_ids_map.end(); fpit++){
    str_val = fpit->first;
    id = fpit->second;
    if(id == search_id ){
      break;
    }
  }
  return str_val;
}
LogsMysql::LogsMysql(std::string domain, std::string mysql_host, int mysql_port, std::string mysql_user, std::string mysql_password) : host(mysql_host), port(mysql_port), username(mysql_user), password(mysql_password), mysql_url("tcp://"+host+":"+std::to_string(port)), domain_name(domain) {
}
void LogsMysql::initThread(){
  driver = sql::mysql::get_driver_instance();
  handler = new st_worker_thread_param;
  handler->driver = driver;
  handler->driver->threadInit();
}
unsigned long LogsMysql::getDomainsId(  std::string domain ){
  unsigned long possible_did=0, real_did=0;
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
      possible_did = (unsigned long)res->getUInt("did");
    }
    else {
      throw std::runtime_error("could not find a possible did for hostname: "+domain);
    }
    sql = "SELECT d1.did as real_did FROM domains d1 JOIN domains d2 on d1.did=d2.alias_of WHERE d1.alias_of=0 AND d1.did="+std::to_string(possible_did)+" OR d2.did="+std::to_string(possible_did)+" LIMIT 1;";
    res.reset(runSelectQuery(stmt,sql));
    if ( res->next() ) {
      real_did =  (unsigned long)res->getUInt("real_did");
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
unsigned long LogsMysql::getUserId( unsigned long real_did ){
  unsigned long uid=0;
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
      uid = (unsigned long)res->getUInt("uid");
      break; // should only be one, break on first
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::getUserId caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
  return uid;
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
std::vector<std::string> LogsMysql::getUserHostnames( unsigned long real_did ){
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
    std::cerr<< "LogsMysql::getUserHostname caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
  return internal_hostnames;
}
void LogsMysql::insertClientIps(std::map<unsigned long,unsigned long> &client_ips_ids, std::map<unsigned long, int> client_ips) {
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
      runQuery(stmt,insert_sql);
      boost::scoped_ptr< sql::ResultSet > res(runSelectQuery(stmt,select_ids_sql));
      while (res->next()) {
        unsigned long ip = (unsigned long)res->getUInt("ipv4");
        unsigned long id = (unsigned long)res->getUInt("id");
        client_ips_ids.insert({ip,id});
      }
    }
  }
  catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertClientIps caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertStringEntities(std::string database, std::string table, std::map<std::string,unsigned long> &entity_ids_map, std::map<std::string, int> entities ) {
  try {
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    sql::mysql::MySQL_Connection * mysql_conn = dynamic_cast<sql::mysql::MySQL_Connection*>(handler->con);
    std::string insert_sql = "INSERT IGNORE INTO "+table+"(name) VALUES ";
    std::string select_ids_sql = "SELECT id,name FROM "+table+" WHERE name IN (";
    std::map<std::string,int>::iterator str_it;
    for( str_it = entities.begin(); str_it!=entities.end(); str_it++){
      std::string name = str_it->first;
      insert_sql += "('"+mysql_conn->escapeString(name).substr(0,250)+"'),";
      select_ids_sql+= "'"+mysql_conn->escapeString(name).substr(0,250)+"',";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1)+";";
    select_ids_sql = select_ids_sql.substr(0, select_ids_sql.size()-1)+");";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( entities.size() > 0){
      runQuery(stmt,insert_sql);
      boost::scoped_ptr< sql::ResultSet > res(runSelectQuery(stmt,select_ids_sql));
      while (res->next()) {
        unsigned long id = (unsigned long) res->getUInt("id");
        std::string name = res->getString("name");
        entity_ids_map.insert({name,id});
      }
    }
  }
  catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertStringEntities caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertExternalDomains(std::map<std::string,unsigned long> &referer_hostnames_ids, std::map<std::string, int> referer_hostnames) {
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
        unsigned long id = (unsigned long) res->getUInt("id");
        std::string name = res->getString("name");
        referer_hostnames_ids.insert({name,id});
      }
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertExternalDomains caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertNameVersionEntities(std::string database, std::string table, std::map<KeyValueContainer,unsigned long> &entity_ids_map, std::map<KeyValueContainer, int> entities) {
  try {
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    sql::mysql::MySQL_Connection * mysql_conn = dynamic_cast<sql::mysql::MySQL_Connection*>(handler->con);
    std::string insert_sql = "INSERT IGNORE INTO "+table+"(name,version) VALUES ";
    std::string select_ids_sql = "SELECT name,version,id FROM "+table+" WHERE ";
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
        unsigned long id = (unsigned long) res->getUInt("id");
        KeyValueContainer nv = KeyValueContainer(name,version);
        entity_ids_map.insert({nv,id});
      }
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertNameVersionEntities caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertSearchTerms( std::map<KeyValueContainer,unsigned long> &search_terms_ids, std::map<KeyValueContainer, int> search_terms, std::map<std::string,unsigned long> referer_hostnames_ids ) {
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
    int i =0;
    for( kv_it = search_terms.begin(); kv_it!=search_terms.end(); kv_it++){
      KeyValueContainer kvC = kv_it->first;
      std::string name = kvC.getKey();
      std::string search_hostname = kvC.getValue();
      std::map<std::string,unsigned long>::iterator it;
      it = referer_hostnames_ids.find(search_hostname);
      int sid = it == referer_hostnames_ids.end() ? 0 : it->second;
      if( sid == 0) {
        throw std::runtime_error( "couldn't find search_hostname: "+search_hostname+" for domain name "+domain_name );
      }
      insert_sql += "('"+mysql_conn->escapeString(name)+"',"+mysql_conn->escapeString(std::to_string(sid))+"),";
      select_ids_sql+= "(name = '"+mysql_conn->escapeString(name)+"' AND search_engine_id= "+mysql_conn->escapeString(std::to_string(sid))+") OR ";
      i++;
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1); // chop off ','
    insert_sql +=";";
    select_ids_sql = select_ids_sql.substr(0, select_ids_sql.size()-3); // chop off last ','
    select_ids_sql +=";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( search_terms.size() > 0 && i>0) {
      runQuery(stmt,insert_sql);
      boost::scoped_ptr< sql::ResultSet > res(runSelectQuery(stmt,select_ids_sql));
      int i =0;
      while (res->next()) {
        unsigned long id = ( unsigned long )res->getUInt("id");
        std::string search_term = res->getString("name");
        std::string search_engine_id = std::to_string((int)res->getUInt("search_engine_id"));
        KeyValueContainer nv = KeyValueContainer(search_term, search_engine_id );
        search_terms_ids.insert({nv,id});
        i++;
      }
    }
  }
  catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertSearchTerms caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertTrafficVectors(bool inner, std::map<TVectorContainer,unsigned long> &tvectors_ids, std::map<TVectorContainer,int> tvectors, std::map<std::string,unsigned long> referer_hostnames_ids, std::map<std::string,unsigned long> page_paths_full_ids, std::string domain_name){
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
    int i=0;
    for( tvc_it = tvectors.begin(); tvc_it!=tvectors.end(); tvc_it++){
      TVectorContainer tvC = tvc_it->first;
      std::string path_a = tvC.getPagePathA();
      std::string path_b = tvC.getPagePathB();
      std::map<std::string,unsigned long>::iterator it = page_paths_full_ids.find(path_a);
      unsigned long path_a_id = it  == page_paths_full_ids.end() ? 0: it->second;
      it = page_paths_full_ids.find(path_b);
      unsigned long path_b_id = it  == page_paths_full_ids.end() ? 0: it->second;
      if( path_a_id == 0 ) {
        throw std::runtime_error( "couldn't find page_path_a: "+path_a+" for domain name "+domain_name );
      }
      if(path_b_id == 0 ) { 
        throw std::runtime_error( "couldn't find page_path_b: "+path_b+" for domain name "+domain_name );
      }
      if(inner){
        insert_values += "("+std::to_string(path_a_id)+","+std::to_string(path_b_id)+"),";
      }
      else {
        std::string external_domain = tvC.getExternalDomain();
        it = referer_hostnames_ids.find(external_domain);
        unsigned long referer_domain_id =it == referer_hostnames_ids.end() ? 0 : it->second;
        if(referer_domain_id ==0) {
          throw std::runtime_error( "couldn't find external_domain: "+external_domain+" for domain name "+domain_name );
        }
        insert_values += "("+std::to_string(referer_domain_id)+","+std::to_string(path_a_id)+","+std::to_string(path_b_id)+"),";
      }
      i++;
    }
    insert_values= insert_values.substr(0, insert_values.size()-1)+";";
    insert_sql += insert_values;
    temp_table_insert_sql += insert_values;
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    boost::scoped_ptr< sql::Statement > create_stmt(handler->con->createStatement());
    if( tvectors.size() > 0 && i>0) {
      runQuery(stmt,insert_sql);
      runQuery(stmt,create_temp_table_sql);
      runQuery(stmt,temp_table_insert_sql);
      boost::scoped_ptr< sql::ResultSet > res(runSelectQuery(stmt,select_ids_sql));
      if( res->rowsCount() < tvectors.size() ) {
        //throw std::runtime_error("rows count < needed for " + table);
      }
      while ( res->next() ) {
        int col_index = 1;
        unsigned long id = (unsigned long) res->getUInt(col_index);
        unsigned long external_domain_id = 0;
        if(!inner) {
          col_index = 2;
          external_domain_id = (unsigned long)res->getUInt(col_index);
        }
        col_index = inner ? 2:3;
        unsigned long a_id =(unsigned long)res->getUInt(col_index);
        col_index = inner ? 3:4;
        unsigned long b_id = (unsigned long)res->getUInt(col_index);
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
void LogsMysql::insertHitsPerHour(std::map<HourlyHitsContainer,int> hits, unsigned long real_did) {
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
void LogsMysql::insertVisitsPerHour( std::map<HourlyVisitsContainer,int> visits, unsigned long real_did, std::map<unsigned long, unsigned long> client_ips_ids ){
  try {
    int i =0;
    std::string database = "httpstats_clients";
    std::string table = "visits_hourly";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    sql::mysql::MySQL_Connection * mysql_conn = dynamic_cast<sql::mysql::MySQL_Connection*>(handler->con);
    std::string insert_sql = "INSERT IGNORE INTO "+mysql_conn->escapeString(table)+"(date,domains_id,ip_id,count) VALUES ";
    std::map<HourlyVisitsContainer,int>::iterator hvc_it;
    for( hvc_it = visits.begin(); hvc_it!=visits.end(); hvc_it++) {
      HourlyVisitsContainer hvC = hvc_it->first;
      unsigned long ip = hvC.getIp();
      int count = hvc_it->second;
      std::map<unsigned long,unsigned long>::iterator it = client_ips_ids.find(ip);
      unsigned long ip_id = it == client_ips_ids.end()? 0 : it->second;
      if( ip_id == 0 ) { 
        throw std::runtime_error( "couldn't find ip " + std::to_string(ip) +" for domain name " + domain_name);
      }
      insert_sql += "('"+mysql_conn->escapeString(hvC.getTsMysql())+"',"+mysql_conn->escapeString(std::to_string(real_did))+","+mysql_conn->escapeString(std::to_string(ip_id))+","+mysql_conn->escapeString(std::to_string(count))+"),";
      i++;
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1) +";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( visits.size() > 0  && i > 0 ) {
      runQuery(stmt,insert_sql);
    }
  }
  catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertVisitsPerHour caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertPageviewsPerHour( std::map<HourlyPageviewsContainer,int> pageviews, unsigned long real_did, std::map<unsigned long, unsigned long> client_ips_ids, std::map<std::string,unsigned long> page_paths_full_ids){
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
      std::map<unsigned long,unsigned long>::iterator it = client_ips_ids.find(ip);
      unsigned long ip_id = it == client_ips_ids.end()? 0 : it->second;
      if( ip_id == 0 ) { 
        throw std::runtime_error( "couldn't find ip " + std::to_string(ip) +" for domain name " + domain_name);
        continue;
      }
      std::map<std::string,unsigned long>::iterator pit = page_paths_full_ids.find(page_path);
      unsigned long page_id = pit  == page_paths_full_ids.end() ? 0: pit->second;
      if( page_id == 0 ) {
        throw std::runtime_error( "couldn't find page_path" + page_path +" for domain name " + domain_name);
      }
      insert_sql += "('"+mysql_conn->escapeString(hpC.getTsMysql())+"',"+mysql_conn->escapeString(std::to_string(real_did))+","+mysql_conn->escapeString(std::to_string(ip_id))+","+mysql_conn->escapeString(std::to_string(page_id))+","+mysql_conn->escapeString(std::to_string(count))+"),";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1)+ ";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( pageviews.size()> 0){
      runQuery(stmt,insert_sql);
    }
  }
  catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertPageviewsPerHour caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertUserAgentEntitiesPerHour( std::map<HourlyUserAgentEntityContainer,int> devices_per_hour, std::map<HourlyUserAgentEntityContainer,int> oses_per_hour, std::map<HourlyUserAgentEntityContainer,int> browsers_per_hour, unsigned long real_did, std::map<KeyValueContainer,unsigned long> client_devices_ids, std::map<KeyValueContainer,unsigned long> client_oses_ids, std::map<KeyValueContainer,unsigned long> client_browsers_ids ){
  try {
    buildAndRunHourlyUAEQuery("devices_hourly", "device_id", devices_per_hour, real_did, client_devices_ids );
    buildAndRunHourlyUAEQuery("oses_hourly", "os_id", oses_per_hour, real_did, client_oses_ids);
    buildAndRunHourlyUAEQuery("browsers_hourly", "browser_id", browsers_per_hour , real_did, client_browsers_ids );
  }
  catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertUserAgentEntitiesPerHour caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertBandwidthPerHour( std::map<HourlyBandwidthContainer,int> bandwidth, unsigned long real_did, std::map<std::string,unsigned long> page_paths_full_ids){
  try {
    std::string database = "httpstats_pages";
    std::string table = "bandwidth_hourly";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    sql::mysql::MySQL_Connection * mysql_conn = dynamic_cast<sql::mysql::MySQL_Connection*>(handler->con);
    std::string insert_sql = "INSERT IGNORE INTO "+ mysql_conn->escapeString(table)+"(date,domains_id,page_id,size_kb) VALUES ";
    std::map<HourlyBandwidthContainer,int>::iterator hbc_it;
    for( hbc_it = bandwidth.begin(); hbc_it!=bandwidth.end(); hbc_it++){
      HourlyBandwidthContainer hbC = hbc_it->first;
      int size_kb = hbC.getSizeInKb( );
      int count = hbc_it->second;
      unsigned long total_size_kb = count * size_kb;
      std::string page_path_full = hbC.getPagePath();
      std::map<std::string,unsigned long>::iterator pit = page_paths_full_ids.find(page_path_full);
      unsigned long page_id = pit  == page_paths_full_ids.end() ? 0: pit->second;
      if( page_id == 0 ) { 
        throw std::runtime_error( "couldn't find page_path" + page_path_full +" for domain name " + domain_name);
      }
      insert_sql += "('"+ mysql_conn->escapeString(hbC.getTsMysql())+"',"+ mysql_conn->escapeString(std::to_string(real_did))+","+ mysql_conn->escapeString(std::to_string(page_id))+","+ mysql_conn->escapeString(std::to_string(total_size_kb ))+"),";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1)+ ";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( bandwidth.size()> 0){
      runQuery(stmt,insert_sql);
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertBandwidthPerHour caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertTVCPerHour( bool is_inner, std::map<HourlyTVContainer,int> tvectors, unsigned long real_did, std::map<TVectorContainer,unsigned long> tvectors_ids ){
  try {
    std::string database = "httpstats_pages";
    std::string table = is_inner ?"tvinn_hourly":"tvinc_hourly";
    std::string tv_column = is_inner ?"tvinn_id":"tvinc_id";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    sql::mysql::MySQL_Connection * mysql_conn = dynamic_cast<sql::mysql::MySQL_Connection*>(handler->con);
    std::string insert_sql = "INSERT IGNORE INTO "+ table+"(date,domains_id,"+tv_column+",count) VALUES ";
    std::map<HourlyTVContainer,int>::iterator htvc_it;
    for( htvc_it = tvectors.begin(); htvc_it!=tvectors.end(); htvc_it++){
      HourlyTVContainer htvc = htvc_it->first;
      int count = htvc_it->second;
      std::map<TVectorContainer,unsigned long>::iterator tvh_it;
      tvh_it = tvectors_ids.find(htvc);
      unsigned long htvc_id=  tvh_it == tvectors_ids.end() ? 0 : tvh_it->second;
      if(htvc_id==0){
        throw std::runtime_error( "couldn't find TVectorContainer "+htvc.toString()+" for domain name "+domain_name);
      }
      insert_sql += "('"+ mysql_conn->escapeString(htvc.getTsMysql())+"',"+ mysql_conn->escapeString(std::to_string(real_did))+","+ mysql_conn->escapeString(std::to_string(htvc_id))+","+ mysql_conn->escapeString(std::to_string(count))+"),";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1)+ ";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( tvectors.size()> 0){
      runQuery(stmt,insert_sql);
    }
  }
  catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertTVCPerHour caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertLocationsPerHour( std::map<HourlyLocationsContainer,int> locations, unsigned long real_did, std::map<std::string,unsigned long> locations_ids ){
  try {
    std::string database = "httpstats_clients";
    std::string table = "locations_hourly";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    sql::mysql::MySQL_Connection * mysql_conn = dynamic_cast<sql::mysql::MySQL_Connection*>(handler->con);
    std::string insert_sql = "INSERT IGNORE INTO "+ mysql_conn->escapeString(table)+"(date,domains_id,country_id,count) VALUES ";
    std::map<HourlyLocationsContainer,int>::iterator hlc_it;
    for( hlc_it = locations.begin(); hlc_it!=locations.end(); hlc_it++){
      HourlyLocationsContainer hlC = hlc_it->first;
      int count = hlc_it->second;
      std::string country = hlC.getCountryCode( );
      std::map<std::string,unsigned long>::iterator it;
      it = locations_ids.find(country);
      unsigned long location_id=  it == locations_ids.end() ? 0 : it->second;
      if( location_id ==0 ){
        throw std::runtime_error( "couldn't find location  for "+country+" for domain name "+domain_name);
      }
      insert_sql += "('"+ mysql_conn->escapeString(hlC.getTsMysql())+"',"+ mysql_conn->escapeString(std::to_string(real_did))+","+ mysql_conn->escapeString(std::to_string(location_id))+","+ mysql_conn->escapeString(std::to_string(count))+"),";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1)+ ";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( locations.size()> 0){
      runQuery(stmt,insert_sql);
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertLocationsPerHour caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertReferersPerHour( std::map<HourlyReferersContainer,int> referers, unsigned long real_did, std::map<std::string,unsigned long> page_paths_full_ids, std::map<std::string,unsigned long> referer_hostnames_ids ){
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
      std::map<std::string,unsigned long>::iterator pit = page_paths_full_ids.find(page_path);
      unsigned long page_id = pit  == page_paths_full_ids.end() ? 0: pit->second;
      if( page_id == 0 ) { 
        throw std::runtime_error( "couldn't find page path "+page_path+" for domain name "+domain_name);
      }
      bool inner = hrC.isInnerReferer();
      unsigned long referer_domain_id;
      if( !inner ){
        std::map<std::string,unsigned long>::iterator rd_it = referer_hostnames_ids.find(referer_domain);
        referer_domain_id = rd_it == referer_hostnames_ids.end() ? 0 : rd_it->second;
        if(referer_domain_id == 0){ 
          throw std::runtime_error( "couldn't find referer_domain "+referer_domain+" for domain name "+domain_name);
        }
      }
      else referer_domain_id = inner;
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
void LogsMysql::insertSearchTermsPerHour( std::map<HourlySearchTermsContainer,int> search_terms, unsigned long real_did, std::map<std::string,unsigned long> page_paths_full_ids, std::map<KeyValueContainer,unsigned long> search_terms_ids, std::map<std::string,unsigned long> referer_hostnames_ids ){
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
      std::map<std::string,unsigned long>::iterator it;
      it = referer_hostnames_ids.find(search_engine_hostname );
      int sid = it == referer_hostnames_ids.end() ? 0 : it->second;
      if( sid == 0) {
        throw std::runtime_error( "couldn't find search_engine id for "+search_engine_hostname+" for domain name "+domain_name);
      }
      KeyValueContainer stc = KeyValueContainer(search_term, std::to_string(sid));
      std::map<std::string,unsigned long>::iterator pit = page_paths_full_ids.find(page_path);
      unsigned long page_id = pit  == page_paths_full_ids.end() ? 0: pit->second;
      if( page_id == 0 ) { 
        throw std::runtime_error("couldn't find page "+page_path+" for domain name "+domain_name);
      }
      unsigned long search_term_id = (search_terms_ids.find(stc))->second;
      int count = hsc_it->second;
      insert_sql += "('"+ mysql_conn->escapeString(hsC.getTsMysql())+"',"+ mysql_conn->escapeString(std::to_string(real_did))+","+ mysql_conn->escapeString(std::to_string(page_id))+","+ mysql_conn->escapeString(std::to_string(search_term_id))+","+ mysql_conn->escapeString(std::to_string(count))+"),";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1)+";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( search_terms.size()> 0){
      runQuery(stmt,insert_sql);
    }
  }
  catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::insertPageviewsPerHour caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::buildAndRunHourlyUAEQuery(std::string aeph_table, std::string entity_id_name, std::map<HourlyUserAgentEntityContainer,int> uae_ph, unsigned long real_did, std::map<KeyValueContainer, unsigned long> user_agent_entity_ids ){
  try {
    std::string database = "httpstats_clients";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    sql::mysql::MySQL_Connection * mysql_conn = dynamic_cast<sql::mysql::MySQL_Connection*>(handler->con);
    std::string insert_sql = "INSERT IGNORE INTO "+aeph_table+ "(date,domains_id,"+entity_id_name+",count) VALUES ";
    std::map<HourlyUserAgentEntityContainer,int>::iterator huec_it;
    for( huec_it= uae_ph.begin(); huec_it!=uae_ph.end(); huec_it++){
      HourlyUserAgentEntityContainer huec = huec_it->first;
      std::string name= huec.getUserAgentEntityName();
      std::string type = huec.getUserAgentEntityType();
      KeyValueContainer user_agent_entity(name,type);
      int count = huec_it->second;
      std::map<KeyValueContainer,unsigned long>::iterator it = user_agent_entity_ids.find(user_agent_entity);
      unsigned long device_id = it == user_agent_entity_ids.end() ? 0 : it->second;
      if(device_id==0){
        std::map<KeyValueContainer, unsigned long>::iterator uea_it;
        throw std::runtime_error("couldn't find id when inserting in table "+aeph_table+" for '"+name+"','"+type+"' for domain "+domain_name+"");
      }
      insert_sql += "('"+ mysql_conn->escapeString(huec.getTsMysql())+"',"+ mysql_conn->escapeString(std::to_string(real_did))+","+ mysql_conn->escapeString(std::to_string(device_id))+","+ mysql_conn->escapeString(std::to_string(count))+"),";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1)+ ";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( uae_ph.size()> 0){
      runQuery(stmt,insert_sql);
    }
  }
  catch (sql::SQLException &e) {
    std::cerr<< "LogsMysql::buildAndRunUAEQuery caught exception: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::endThread(){
  handler->driver->threadEnd();
  delete(handler);
}
LogsMysql::~LogsMysql(){
}
