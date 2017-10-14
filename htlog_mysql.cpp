#include "htlog_mysql.hpp"
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
    std::string log_path = "logs/"+domain_name+".sql";
    std::ofstream logfile(log_path, std::fstream::in | std::fstream::out | std::fstream::app );
    logfile<<sql<<std::endl;
    logfile.close();
  }
  sql::ResultSet * res(stmt->executeQuery(sql));
  return res;
}
void LogsMysql::runInsertQuery(boost::scoped_ptr< sql::Statement > & stmt, std::string sql) {
  if(LOG_SQL_STMTS) {
    std::string log_path = "logs/"+domain_name+".sql";
    std::ofstream logfile(log_path, std::fstream::in | std::fstream::out | std::fstream::app );
    logfile<<sql<<std::endl;
    logfile.close();
  }
  stmt->execute(sql);
}
int LogsMysql::getDomainsId(  std::string domain_name ){
  int did=-1;
  std::string database = "cluster";
  std::string real_domain_name;
  try {
    //std::cout<<"mysql_url: "<<mysql_url<<" username: "<<username<<" password: "<<password<<"\n";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    std::string sql= "SELECT did FROM domains where domain_name ='"+ domain_name + "';";
    boost::scoped_ptr< sql::ResultSet > res( runSelectQuery( stmt, sql ) );
    while (res->next()) {
      did = std::stoi( res->getString("did"));
      break; // should only be one, break on first
    }
    sql = "SELECT domain_name FROM domains WHERE alias_of="+std::to_string(did)+" OR did="+std::to_string(did)+";";
    res.reset(runSelectQuery(stmt,sql));
    while (res->next()) {
      real_domain_name = res->getString("domain_name");
      break; // should only be one, break on first
    }
    sql= "SELECT did FROM domains where domain_name ='"+ real_domain_name + "';";
    res.reset(runSelectQuery(stmt,sql));
    while (res->next()) {
      did = std::stoi( res->getString("did"));
      break; // should only be one, break on first
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
  return did;
}
int LogsMysql::getUserId( int real_did ){
  int uid=-1;
  std::string database = "cluster";
  try {
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    std::string sql= "SELECT uid FROM domains where did ="+ std::to_string(real_did)+ ";";
    boost::scoped_ptr< sql::ResultSet > res(runSelectQuery(stmt,sql));
    while (res->next()) {
      uid = std::stoi( res->getString("uid"));
      break; // should only be one, break on first
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
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
    for( ul_it = client_ips.begin(); ul_it!=client_ips.end(); ul_it++){
        unsigned long ip= ul_it->first;
        insert_sql += "("+std::to_string(ip)+"),";
        select_ids_sql+= std::to_string(ip)+",";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1); // chop off last ','
    insert_sql +=";";
    select_ids_sql = select_ids_sql.substr(0, select_ids_sql.size()-1); // chop off last ','
    select_ids_sql +=");";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if(client_ips.size() > 0){
      //std::cout<<"client ips insert_sql: "<<insert_sql<<"\n";
      runInsertQuery(stmt,insert_sql);
      //std::cout<<"client ips select_ids_sql: "<<select_ids_sql<<"\n";
      boost::scoped_ptr< sql::ResultSet > res(runSelectQuery(stmt,select_ids_sql));
      while (res->next()) {
        unsigned long ip = std::stol( res->getString("ipv4"));
        int id = std::stoi( res->getString("id"));
        client_ips_ids.insert({ip,id});
      }
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertStringEntities(std::string database, std::string table, std::map<std::string,int> &entity_ids_map, std::map<std::string, int> entities ) {
  try {
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
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
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if(entities.size() > 0){
      //std::cout<<database<<"."<<table<<" insert_sql: "<<insert_sql<<"\n";
      runInsertQuery(stmt,insert_sql);
      //std::cout<<database<<"."<<table<<"select_ids_sql: "<<select_ids_sql<<"\n";
      boost::scoped_ptr< sql::ResultSet > res(runSelectQuery(stmt,select_ids_sql));
      while (res->next()) {
        std::string name = res->getString("name");
        int id = std::stoi( res->getString("id"));
        entity_ids_map.insert({name,id});
      }
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertNameVersionEntities(std::string database, std::string table, std::map<KeyValueContainer,int> &entity_ids_map, std::map<KeyValueContainer, int> entities) {
  try {
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
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
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if(entities.size() > 0){
      //std::cout<<database<<"."<<table<<" insert_sql: "<<insert_sql<<"\n";
      runInsertQuery(stmt,insert_sql);
      //std::cout<<database<<"."<<table<<"select_ids_sql: "<<select_ids_sql<<"\n";
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
    std::cerr<< "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertSearchTerms(std::map<KeyValueContainer,int> &entity_ids_map, std::map<KeyValueContainer, int> entities, std::map<std::string,int> referer_hostnames_ids) {
  try {
    std::string database = "httpstats_pages";
    std::string table = "search_terms";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    std::string insert_sql = "INSERT IGNORE INTO "+table+"(name,search_engine_id) VALUES ";
    std::string select_ids_sql = "SELECT name,search_engine_id,id FROM "+table+" WHERE ";
    std::map<KeyValueContainer,int>::iterator kv_it;
    for( kv_it = entities.begin(); kv_it!=entities.end(); kv_it++){
      KeyValueContainer kvC = kv_it->first;
      std::string name = kvC.getKey();
      std::string search_hostname = kvC.getValue();
      int sid =  (referer_hostnames_ids.find(search_hostname))->second;
      //std::cout<<"sid "<<std::to_string(sid)<<" search_host "<<search_hostname<<"\n";
      insert_sql += "('"+name+"',"+std::to_string(sid)+"),";
      select_ids_sql+= "(name = '"+name+"' AND search_engine_id= "+std::to_string(sid)+") OR ";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1); // chop off ','
    insert_sql +=";";
    select_ids_sql = select_ids_sql.substr(0, select_ids_sql.size()-3); // chop off last ','
    select_ids_sql +=";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( entities.size() > 0 ) {
      //std::cout<<database<<"."<<table<<" insert_sql: "<<insert_sql<<"\n";
      runInsertQuery(stmt,insert_sql);
      //std::cout<<database<<"."<<table<<"select_ids_sql: "<<select_ids_sql<<"\n";
      boost::scoped_ptr< sql::ResultSet > res(runSelectQuery(stmt,select_ids_sql));
      int i =0;
      //if(res->rowsCount() != entities.size()){
      //throw std::runtime_error( "rows count != entities number "+std::to_string(res->rowsCount())+ "!="+std::to_string(entities.size())+ "\n" + "insert_sql: " + insert_sql + "\n" + "select_ids_sql: " + select_ids_sql + "\n" ); 
      //}
      while (res->next()) {
        KeyValueContainer nv= getNthNode(entities,i);
        int id = std::stoi( res->getString("id"));
        //std::cout<<"entity_ids_map insert "<<nv.toString()<<","<<id<<"\n";
        entity_ids_map.insert({nv,id});
        i++;
      }
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
  //} catch ( std::runtime_error &e) {
  //std::cerr<< "# ERR: " << e.what() << "\n";
  //}
}
void LogsMysql::insertParamsEntities(std::map<ParamsContainer,int> &entity_ids_map, std::map<ParamsContainer,int> entities, std::map<std::string,int> page_paths_ids, std::map<std::string,int> page_paths_full_ids) {
  try {
    std::string database = "httpstats_pages";
    std::string table = "url_params";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
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
      //std::cout<<pC.toString()<<"\n";
      int full_path_id = page_paths_full_ids.find(full_path)->second;
      insert_sql += "("+std::to_string(path_id)+","+std::to_string(full_path_id)+",'"+key+"','"+value+"'),";
      select_ids_sql+= "(full_page_id = "+std::to_string(full_path_id)+" AND page_id = "+std::to_string(path_id)+" AND k = '"+key+"' AND val = '"+value+"' ) OR ";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1); // chop off ','
    insert_sql +=";";
    select_ids_sql = select_ids_sql.substr(0, select_ids_sql.size()-3); // chop off last ','
    select_ids_sql +=";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if(entities.size() > 0 ){
      //std::cout<<database<<"."<<table<<" insert_sql: "<<insert_sql<<"\n";
      runInsertQuery(stmt,insert_sql);
      boost::scoped_ptr< sql::ResultSet > res(runSelectQuery(stmt,select_ids_sql));
      //std::cout<<database<<"."<<table<<"select_ids_sql: "<<select_ids_sql<<"\n";
      int i =0;
      if(res->rowsCount() != entities.size()){
        //throw std::runtime_error( "rows count != entities number "+std::to_string(res->rowsCount())+"!="+std::to_string(entities.size())+ "insert_sql: " + insert_sql + "\n" + "select_ids_sql: " + select_ids_sql + "\n" );
      }
      while (res->next()) {
        int id = std::stoi( res->getString("id"));
        ParamsContainer found_pc = getNthNode(entities,i);
        entity_ids_map.insert({found_pc,id});
        i++;
      }
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }/* catch ( std::runtime_error &e) {
    std::cerr<< "# ERR: " << e.what() << "\n";
  }*/
}
void LogsMysql::insertExternalDomains(std::map<std::string,int> &referer_hostnames_ids, std::map<std::string, int> referer_hostnames) {
  try {
    std::string database = "httpstats_pages";
    std::string table = "external_domains";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
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
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( referer_hostnames.size() > 0 ){
      //std::cout<<database<<"."<<table<<" insert_sql: "<<insert_sql<<"\n";
      runInsertQuery(stmt,insert_sql);
      //std::cout<<database<<"."<<table<<"select_ids_sql: "<<select_ids_sql<<"\n";
      boost::scoped_ptr< sql::ResultSet > res(runSelectQuery(stmt,select_ids_sql));
      while (res->next()) {
        std::string name = res->getString("name");
        int id = std::stoi( res->getString("id"));
        referer_hostnames_ids.insert({name,id});
      }
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertTrafficVectors(bool inner, std::map<TVectorContainer,int> &tvectors_ids, std::map<TVectorContainer,int> tvectors, std::map<std::string,int> referer_hostnames_ids, std::map<std::string,int> page_paths_full_ids){
  try {
    std::string database = "httpstats_pages";
    std::string table = inner ? "tvectors_inn" : "tvectors_inc";
    std::string insert_sql = inner ? 
      "INSERT IGNORE INTO "+table+"(a_id,b_id) VALUES ":
      "INSERT IGNORE INTO "+table+"(referer_domain_id,referer_page_id,page_id) VALUES " ;
    std::string select_ids_sql = inner ? 
      "SELECT id,a_id,b_id FROM "+table+" WHERE ":
      "SELECT id,referer_domain_id,referer_page_id,page_id FROM "+table+" WHERE ";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    std::map<TVectorContainer,int>::iterator tvc_it;
    for( tvc_it = tvectors.begin(); tvc_it!=tvectors.end(); tvc_it++){
      TVectorContainer tvC = tvc_it->first;
      std::string path_a = tvC.getPagePathA();
      std::string path_b = tvC.getPagePathB();
      int path_a_id = page_paths_full_ids.find(path_a)->second;
      int path_b_id = page_paths_full_ids.find(path_b)->second;
      if(inner){
        insert_sql += "("+std::to_string(path_a_id)+","+std::to_string(path_b_id)+"),";
        select_ids_sql+= "(a_id= "+std::to_string(path_a_id)+" AND b_id = "+std::to_string(path_b_id)+" ) OR ";
      }
      else {
        std::string external_domain = tvC.getExternalDomain();
        int referer_domain_id = referer_hostnames_ids.find(external_domain)->second;
        insert_sql += "("+std::to_string(referer_domain_id)+","+std::to_string(path_a_id)+","+std::to_string(path_b_id)+"),";
        select_ids_sql+= "(referer_domain_id="+std::to_string(referer_domain_id)+" AND referer_page_id = "+std::to_string(path_a_id)+" AND page_id = "+std::to_string(path_b_id)+" ) OR ";
      }
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1); // chop off ','
    insert_sql +=";";
    select_ids_sql = select_ids_sql.substr(0, select_ids_sql.size()-3); // chop off last ','
    select_ids_sql +=";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( tvectors.size() ) {
      //std::cout<<database<<"."<<table<<" insert_sql: "<<insert_sql<<"\n";
      runInsertQuery(stmt,insert_sql);
      //std::cout<<database<<"."<<table<<"select_ids_sql: "<<select_ids_sql<<"\n";
      boost::scoped_ptr< sql::ResultSet > res(runSelectQuery(stmt,select_ids_sql));
      int i =0;
      if(res->rowsCount() != tvectors.size()){
        //throw std::runtime_error( "rows count != tvectors number "+std::to_string(res->rowsCount())+"!="+std::to_string(tvectors.size())+ "insert_sql: " + insert_sql + "\n" + "select_ids_sql: " + select_ids_sql + "\n" );
      }
      while (res->next()) {
        int id = std::stoi( res->getString("id"));
        TVectorContainer found = getNthNode(tvectors,i);
        tvectors_ids.insert({found,id});
        i++;
      }
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }/* catch ( std::runtime_error &e) {
    std::cerr<< "# ERR: " << e.what() << "\n";
  }*/
}
void LogsMysql::insertHitsPerHour(std::map<HourlyHitsContainer,int> hits, int real_did){
  try {
    std::string database = "httpstats_domains";
    std::string table = "hits_hourly";
    std::string insert_sql = "INSERT IGNORE INTO "+table+"(date,domains_id,count) VALUES ";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    std::map<HourlyHitsContainer,int>::iterator hhc_it;
    for( hhc_it = hits.begin(); hhc_it!=hits.end(); hhc_it++){
      HourlyHitsContainer hhC = hhc_it->first;
      int count = hhc_it->second;
      insert_sql += "('"+hhC.getTsMysql()+"',"+std::to_string(real_did)+","+std::to_string(count)+"),";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1);
    insert_sql +=";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( hits.size()>0 ){
      //std::cout<<database<<"."<<table<<" insert_sql: "<<insert_sql<<"\n";
      runInsertQuery(stmt,insert_sql);
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertVisitsPerHour( std::map<HourlyVisitsContainer,int> visits, int real_did, std::map<unsigned long, int> client_ips_ids ){
  try {
    std::string database = "httpstats_clients";
    std::string table = "visits_hourly";
    std::string insert_sql = "INSERT IGNORE INTO "+table+"(date,domains_id,ip_id,count) VALUES ";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    std::map<HourlyVisitsContainer,int>::iterator hvc_it;
    for( hvc_it = visits.begin(); hvc_it!=visits.end(); hvc_it++){
      HourlyVisitsContainer hvC = hvc_it->first;
      unsigned long ip = hvC.getIp();
      int count = hvc_it->second;
      int ip_id =  (client_ips_ids.find(ip))->second;
      insert_sql += "('"+hvC.getTsMysql()+"',"+std::to_string(real_did)+","+std::to_string(ip_id)+","+std::to_string(count)+"),";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1);
    insert_sql +=";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( visits.size() > 0 ) {
      //std::cout<<database<<"."<<table<<" insert_sql: "<<insert_sql<<"\n";
      runInsertQuery(stmt,insert_sql);
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
void LogsMysql::insertPageviewsPerHour( std::map<HourlyPageviewsContainer,int> pageviews, int real_did, std::map<unsigned long, int> client_ips_ids, std::map<std::string,int> page_paths_full_ids ){
  try {
    std::string database = "httpstats_pages";
    std::string table = "pageviews_hourly";
    std::string insert_sql = "INSERT IGNORE INTO "+table+"(date,domains_id,ip_id,page_id,count) VALUES ";
    boost::scoped_ptr< sql::Connection > conn(handler->driver->connect(mysql_url, username, password));
    handler->con = conn.get();
    handler->con->setSchema(database);
    std::map<HourlyPageviewsContainer,int>::iterator hpc_it;
    for( hpc_it = pageviews.begin(); hpc_it!=pageviews.end(); hpc_it++){
      HourlyPageviewsContainer hpC = hpc_it->first;
      unsigned long ip = hpC.getIp();
      std::string page_path = hpC.getPagePath();
      int count = hpc_it->second;
      int ip_id =  (client_ips_ids.find(ip))->second;
      int page_id =  (page_paths_full_ids.find(page_path))->second;
      insert_sql += "('"+hpC.getTsMysql()+"',"+std::to_string(real_did)+","+std::to_string(ip_id)+","+std::to_string(page_id)+","+std::to_string(count)+"),";
    }
    insert_sql = insert_sql.substr(0, insert_sql.size()-1);
    insert_sql +=";";
    boost::scoped_ptr< sql::Statement > stmt(handler->con->createStatement());
    if( pageviews.size()> 0){
      //std::cout<<database<<"."<<table<<" insert_sql: "<<insert_sql<<"\n";
      runInsertQuery(stmt,insert_sql);
    }
  } catch (sql::SQLException &e) {
    std::cerr<< "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )\n";
  }
}
LogsMysql::~LogsMysql(){
}
