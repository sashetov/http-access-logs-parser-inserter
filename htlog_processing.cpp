#include "htlog_processing.hpp"
//EXTERN GLOBALS
extern std::string mysql_hostname, mysql_port_num, mysql_user, mysql_password, dirname, logfile;
extern std::vector<SearchEngineContainer> search_hosts;
extern std::vector<std::string> filenames;
//PTHREAD GLOBALS
std::mutex m;
int id;
Timer * threads_timer = new Timer();
int worker_threads_launched_total=0;
int worker_threads_working=0;
//MISC PRE-INIT
std::string getHostnameFromLogfile( std::string filename ){
  std::string hostname = "";
  std::string line;
  try {
    std::ifstream statsFile;
    int spaceLocation;
    statsFile.exceptions( std::ifstream::badbit | std::ifstream::failbit);
    statsFile.open(filename);
    while( !statsFile.eof() ){
      std::getline(statsFile, line);
      if(line.length()>0){
        if ((spaceLocation = line.find_first_of(" ")) == -1){
          break;
        }
        hostname = line.substr(0, spaceLocation);
      }
      break;
    }
    statsFile.close();
  } catch( const std::exception& e ){
    std::cerr<<"getHostnameFromLogfile caught exception: "<<e.what()<<"\n";
  }
  return hostname;
}
//PUBLIC
HttpAccessLogMetrics::HttpAccessLogMetrics( std::string user_host, std::vector<SearchEngineContainer> search_hosts, std::string file ) : lm(user_host,mysql_hostname,std::stoi(mysql_port_num),mysql_user,mysql_password){
  search_engines = search_hosts;
  lines_failed=0;
  lines_processed=0;
  lm.initThread();
  real_did = lm.getDomainsId(user_host);
  internal_hostnames = lm.getUserHostnames( real_did );
  uid = lm.getUserId( real_did );
  lm.endThread();
  filename = file;
  timer = new Timer();
}
HttpAccessLogMetrics::~HttpAccessLogMetrics(){
  delete timer;
}
int HttpAccessLogMetrics::getDomainId(){
  return real_did;
}
int HttpAccessLogMetrics::logsScan( ){
  std::ifstream statsFile;
  std::string line;
  try {
    statsFile.exceptions( std::ifstream::badbit | std::ifstream::failbit);
    statsFile.open(filename);
    // don't use [] operator to check for \0 - segfaults for some reason..
    if (filename == "-") {
      return 1;
    }
    while ( !statsFile.eof() ){
      parsed_logline ll;
      std::getline(statsFile, line);
      if (parseLine(line,ll) == 0) {
        HourlyHitsContainer hhc = HourlyHitsContainer( real_did , ll.timestamp );
        HourlyVisitsContainer hvc = HourlyVisitsContainer( real_did, ll.timestamp, ll.userIP);
        HourlyPageviewsContainer hpc = HourlyPageviewsContainer( real_did, ll.timestamp, ll.userIP, ll.requestPath );
        incrementCount( &hits, hhc );
        incrementCount( &visits, hvc );
        incrementCount( &pageviews, hpc );
        incrementCount( &client_ips, ll.userIP);
        incrementCount( &client_geo_locations, getCountryFromIP(ll.userIPStr));
        processUserAgent( ll.agent );
        processTrafficVectors( ll.requestPath, ll.referer );
        processRequestUrl(ll.requestPath);
        processSearchTerms(ll.referer,ll.timestamp);
        processReferers(ll.referer,ll.timestamp);
        lines_processed++;
      }
      else {
        lines_failed++;
        std::cerr<<"Invalid line: "<< line<<"\n";
      }
    }
    statsFile.close();
  } catch (const std::exception& e) {
    if(!statsFile.eof()){
      std::cerr<<"HttpAccessLogMetrics::logsScan caught exception: "<<e.what()<<"\n while processing filename "<<filename<<std::endl;
    }
    timer->stop("logsScan");
    return 6;
  }
  return 0;
}
unsigned long HttpAccessLogMetrics::getNumericIp( std::string addr ){
  uint32_t nbo_addr = inet_addr(addr.c_str());//address in network byte order
  return (unsigned long) ntohl(nbo_addr);
}
std::string HttpAccessLogMetrics::getStringIP( unsigned long ip ){
  unsigned long hostlong = htonl(ip);
  struct in_addr ip_addr;
  ip_addr.s_addr = hostlong;
  return std::string(inet_ntoa(ip_addr));
}
time_t HttpAccessLogMetrics::getTimestamp( std::string dateTime ){
  struct tm tm;
  time_t epoch;
  if ( strptime(dateTime.c_str(), "%d/%h/%Y:%H:%M:%S %Z", &tm) != NULL ){
    epoch = mktime(&tm);
  }
  else {
    return -1;
  }
  return epoch;
}
int HttpAccessLogMetrics::parseLine( std::string line, parsed_logline &ll ){
  std::string host, userIP, date, hour, timezone, request, requestType, requestPath, requestProtocol, referer, userAgent, size, status;
  int statusCode;
  long sizeBytes;
  int uIPCursor0, uIPCursor1, dateCursor0, dateCursor1, reqCursor0, reqCursor1, reqUrlCursor0, reqUrlCursor1, refCursor0, refCursor1, uaCursor0, uaCursor1, statusCursor0, statusCursor1, sizeCursor0, sizeCursor1;
  //USER IP
  if ((uIPCursor0= line.find_first_of(" ")) == -1){
    return 1;
  }
  //HOST
  host = line.substr( 0, uIPCursor0 );
  if ((uIPCursor1= line.find_first_of(" ", uIPCursor0+1) )== -1){
    return 2;
  }
  userIP = line.substr(uIPCursor0+1,uIPCursor1-uIPCursor0);
  //DATE
  if ((dateCursor0 = line.find_first_of("[")) == -1){
    return 3;
  }
  if ((dateCursor1 = line.find_first_of("]",dateCursor0+1)) == -1){
    return 4;
  }
  date = line.substr( dateCursor0+1, dateCursor1-dateCursor0-1 );
  //REQ
  if ((reqCursor0 = line.find_first_of("\"")) == -1){
    return 5;
  }
  if ((reqCursor1 = line.find_first_of("\"",reqCursor0+1)) == -1){
    return 6;
  }
  request = line.substr( reqCursor0+1, reqCursor1-reqCursor0-1 );
  if ((reqUrlCursor0 = request.find_first_of(" ")) == -1){
    return 7;
  }
  if ((reqUrlCursor1= request.find_last_of(" ")) == -1){
    return 8;
  }
  requestPath = request.substr(reqUrlCursor0+1, reqUrlCursor1 -reqUrlCursor0 -1);
  requestType = request.substr(0,reqUrlCursor0);
  requestProtocol= request.substr(reqUrlCursor1+1, request.size()-reqUrlCursor1-1);
  //REFERRER
  if ((refCursor0= line.find_first_of("\"", reqCursor1+1)) == -1){
    return 9;
  }
  if ((refCursor1= line.find_first_of("\"",refCursor0+1)) == -1){
    return 10;
  }
  referer= line.substr( refCursor0+1, refCursor1-refCursor0-1 );
  //USER AGENT
  if ((uaCursor0= line.find_first_of("\"", refCursor1+1)) == -1){
    return 11;
  }
  if ((uaCursor1= line.find_first_of("\"", uaCursor0+1)) == -1){
    return 12;
  }
  userAgent = line.substr(uaCursor0+1, uaCursor1-uaCursor0-1);
  //STATUSCODE
  if ((statusCursor0= line.find_first_of(" ", reqCursor1+1)) == -1){
    return 13;
  }
  if ((statusCursor1= line.find_first_of(" ", statusCursor0+1)) == -1){
    return 14;
  }
  status = line.substr(statusCursor0+1, statusCursor1-statusCursor0-1);
  statusCode = std::stoi(status);
  //SIZE IN BYTES
  if ((sizeCursor0 = line.find_first_of(" ", statusCursor1)) == -1){
    return 15;
  }
  if ((sizeCursor1 = line.find_first_of(" ", sizeCursor0+1)) == -1){
    return 16;
  }
  size= line.substr(sizeCursor0+1, sizeCursor1-sizeCursor0-1);
  sizeBytes = std::stol(size);
  ll.hostname = host;
  ll.userIPStr = userIP;
  ll.userIP= getNumericIp(userIP);
  ll.date= date;
  ll.timestamp = getTimestamp(date);
  //ll->hour= host;
  //ll->timezone= timezone;
  ll.requestPath = requestPath;
  ll.requestType = requestType;
  ll.referer = referer;
  ll.agent = userAgent;
  ll.sizeBytes = sizeBytes;
  ll.statusCode= statusCode;
  /*std::cout <<"hostname:"<<host<<"\n" <<"userIP:"<<userIP<<"\n" <<"date:"<<date<<"\n" <<"request:"<<request<<"\n" <<"requestPath:"<<requestPath<<"\n" <<"requestType:"<<requestType<<"\n" <<"requestProtocol:"<<requestProtocol<<"\n" <<"referer:"<<referer<<"\n" <<"userAgent:"<<userAgent<<"\n" <<"statusCode:"<<statusCode<<"\n" <<"sizeBytes:"<<sizeBytes<<"\n" ;*/
  /*std::cout <<"ll->hostname:"<<ll.hostname<<"\n" <<"userIP:"<<userIP<<"\n" <<"ll->userIP:"<<ll.userIP<<"\n" <<"ll->date:"<<ll.date<<"\n" <<"ll->timestamp:"<<ll.timestamp<<"\n" <<"ll->requestPath:"<<ll.requestPath<<"\n" <<"ll->requestType:"<<ll.requestType<<"\n" <<"ll->referer:"<<ll.referer<<"\n" <<"ll->agent:"<<ll.agent<<"\n" <<"ll->sizeBytes:"<<ll.sizeBytes<<"\n" <<"ll->statusCode:"<<ll.statusCode<<"\n" ;*/
  return 0;
}
template<typename T> void HttpAccessLogMetrics::incrementCount( std::map<T,int> *kvmap, T key ){
  int count=1;
  typename std::map<T, int>::iterator it = kvmap->find(key);
  if (it == kvmap->end()){ //nothing was found
    //std::cout<<"inserting "<<key<<" count "<<count<<"\n";
    kvmap->insert({key,count});
  }
  else {
    it->second += count;
    //std::cout<<"inserting "<<key<<" count "<<it->second<<"\n";
  }
}
std::string HttpAccessLogMetrics::getCountryFromIP( std::string client_ip){
  GeoLite2PP::DB db( "GeoLite2-City.mmdb" );
  GeoLite2PP::MStr m = db.get_all_fields( client_ip );
  return m["country_iso_code"];
}
void HttpAccessLogMetrics::processUserAgent( std::string uaStr ){
  const UserAgent ua = g_ua_parser.parse( uaStr );
  KeyValueContainer deviceC = KeyValueContainer(ua.device.model, ua.device.brand);
  incrementCount( &client_devices, deviceC);
  KeyValueContainer osC = KeyValueContainer(ua.os.family, ua.os.toVersionString());
  incrementCount( &client_oses, osC );
  KeyValueContainer browsersC = KeyValueContainer(ua.browser.family, ua.browser.toVersionString());
  incrementCount( &client_browsers, browsersC );
}
void HttpAccessLogMetrics::processTrafficVectors( std::string requestPath, std::string referer ){
  int i,j;
  std::vector<std::string>::iterator it;
  std::vector<ParamsContainer> params;
  std::vector<ParamsContainer>::iterator params_it;
  int hostname_type= 0; // 0 nothing found, 1 found internal ref, 2 found external ref, 3 found search ref
  url_parts url_parsed = getUrlParts( referer );
  incrementCount(&page_paths_full, url_parsed.full_path);
  std::string hostname;
  for (  it = internal_hostnames.begin(); it != internal_hostnames.end(); it++ ) {
    if( url_parsed.hostname == *it){
      hostname_type = 1;
      break;
    }
    else {
      hostname_type = 2;
    }
  }
  for ( i=0; i< (int) search_engines.size(); i++){
    if( url_parsed.hostname == search_engines[i].hostname ){
      hostname_type = 3;
      break;
    }
  }
  if(hostname_type == 1 ) {
    if( url_parsed.hostname.length() > 0 ) {
      incrementCount(&internal_domains, url_parsed.hostname);
      TVectorContainer inner_tvc(true,"",url_parsed.full_path,requestPath);
      incrementCount(&tvectors_inner, inner_tvc);
    }
    if( url_parsed.path.length() > 0 ) {
      incrementCount(&internal_paths, url_parsed.path );
    }
  }
  else if(hostname_type == 2 ) {
    if( url_parsed.hostname.length() > 0 ) {
      incrementCount(&referer_hostnames, url_parsed.hostname);
      TVectorContainer inner_tvc(false,url_parsed.hostname,url_parsed.full_path,requestPath);
      incrementCount(&tvectors_incoming, inner_tvc);
    }
    if( url_parsed.path.length() > 0 ) {
      incrementCount(&referer_paths, url_parsed.full_path);
    }
  }
  if(hostname_type == 3 ) {
    incrementCount(&referer_hostnames, url_parsed.hostname);
    if( url_parsed.params.length() > 0 && url_parsed.path.length() > 0 && url_parsed.hostname.length() > 0 ) {
      params = parseParamsString( url_parsed.params, hostname_type, url_parsed.hostname, url_parsed.path, referer);
      TVectorContainer inner_tvc(false,url_parsed.hostname,url_parsed.full_path,requestPath);
      incrementCount(&tvectors_incoming, inner_tvc);
    }
    for( params_it = params.begin(); params_it!=params.end(); params_it++){
      std::string params_key =(*params_it).getKey();
      for(i =0; i< (int) search_engines.size(); i++){
        for(j =0; j< (int) search_engines[i].query_params.size(); j++){
          if( params_key == search_engines[i].query_params[j] ){
            std::string search_term=(*params_it).getValue();
            std::string search_engine_hostname = (*params_it).getHost();
            if(search_term.length()> 0){
              KeyValueContainer searchContainer(search_term, search_engine_hostname);
              incrementCount(&search_queries, searchContainer);
              incrementCount(&referer_hostnames, search_engine_hostname );
            }
          }
        }
      }
    }
  }
}
void HttpAccessLogMetrics::processRequestUrl( std::string requestPath ){
  std::vector<ParamsContainer> params;
  std::vector<ParamsContainer>::iterator params_it;
  url_parts url_parsed = getUrlPartsFromReqPath( requestPath, "http", internal_hostnames[0] );
  if( url_parsed.path.length()> 0){
    incrementCount(&page_paths_full, requestPath);
    incrementCount(&internal_paths, url_parsed.full_path);
  }
  if( url_parsed.hostname.length()>0){
    incrementCount(&internal_domains, url_parsed.hostname);
  }
}
void HttpAccessLogMetrics::processSearchTerms( std::string referer_str, time_t timestamp  ){
  int i,j,k;
  url_parts referer_parts = getUrlParts(referer_str);
  std::string search_term;// process search terms
  for ( i =0; i< (int)search_engines.size(); i++) {
    if( referer_parts.hostname == search_engines[i].hostname ) {
      std::vector<ParamsContainer> params = parseParamsString( referer_parts.params, 3, referer_parts.hostname, referer_parts.path,  referer_str);
      for( j=0; j < (int) params.size(); j++ ){
        ParamsContainer param = params[j];
        for( k=0; k < (int) search_engines[i].query_params.size(); k++ ){
          if( param.getKey() == search_engines[i].query_params[k] ){
            search_term = param.getValue();
            break;
          }
        }
      }
      if( search_term.length() > 0 ){
        HourlySearchTermsContainer hsc = HourlySearchTermsContainer( real_did, referer_parts.hostname, search_term, referer_parts.full_path, timestamp);
        incrementCount( &search_terms, hsc );
      }
    }
  }
}
void HttpAccessLogMetrics::processReferers( std::string referer_str, time_t timestamp ){
  int i;
  url_parts referer_parts = getUrlParts(referer_str);
  if(referer_parts.hostname.length() > 0 && referer_parts.full_path.length()> 0) {
    bool inner_hostname = false;
    for(i=0; i < (int)internal_hostnames.size(); i++){
      if(internal_hostnames[i] == referer_parts.hostname){
        inner_hostname=true;
        break;
      }
    }
    HourlyReferersContainer hrc = HourlyReferersContainer( real_did, referer_parts.hostname, referer_parts.full_path, timestamp, inner_hostname );
    incrementCount( &referers, hrc );
  }
}
std::vector<ParamsContainer> HttpAccessLogMetrics::parseParamsString( std::string params_str, int type, std::string hostname, std::string page_path, std::string page_path_full ){
  std::vector<ParamsContainer> results;
  long long found;
  if( params_str.length() > 0 ) { 
    std::string workstring(params_str);
    std::string param;
    while((found = (long long)workstring.find("&") )!= -1 ){
      std::string query;
      param = workstring.substr(0,found);
      workstring = workstring.substr(found+1, workstring.length()-found-1);
      if((found = (long long)param.find("=")) != -1 ){
        std::string key = param.substr(0,found);
        std::string val = param.substr(found+1, param.length()-found-1);
        results.push_back(ParamsContainer(type,hostname,page_path,page_path_full,key,val));
      }
    }
    param = workstring;
    if((found = (long long) param.find("=")) != -1 ){
      std::string key = param.substr(0,found);
      std::string val = param.substr(found+1, param.length()-found-1);
      results.push_back(ParamsContainer(type,hostname,page_path,page_path_full,key,val));
    }
  }
  return results;
}
void HttpAccessLogMetrics::insertEntities(){
  timer->start("initThread");                lm.initThread();                                                                                                       timer->stop("initThread");
  timer->start("insertClientIps");           lm.insertClientIps( client_ips_ids, client_ips );                                                                      timer->stop("insertClientIps");
  timer->start("insertLocations");           lm.insertStringEntities( "httpstats_clients", "locations", client_geo_locations_ids, client_geo_locations );           timer->stop("insertLocations");
  timer->start("insertDevices");             lm.insertNameVersionEntities( "httpstats_clients", "devices", client_devices_ids, client_devices);                     timer->stop("insertDevices");
  timer->start("insertOses");                lm.insertNameVersionEntities( "httpstats_clients", "oses", client_oses_ids, client_oses);                              timer->stop("insertOses");
  timer->start("insertBrowsers");            lm.insertNameVersionEntities( "httpstats_clients", "browsers", client_browsers_ids, client_browsers );                 timer->stop("insertBrowsers");
  timer->start("insertPagePathsFull");       lm.insertStringEntities( "httpstats_pages", "pages_paths_full", page_paths_full_ids, page_paths_full );                timer->stop("insertPagePathsFull");
  timer->start("insertExternalDomains");     lm.insertStringEntities( "httpstats_pages", "external_domains", referer_hostnames_ids, referer_hostnames );            timer->stop("insertExternalDomains");
  timer->start("insertRefererPaths");        lm.insertStringEntities( "httpstats_pages", "pages_paths_full", referer_paths_ids, referer_paths );                    timer->stop("insertRefererPaths");
  timer->start("insertSearchQueries");       lm.insertSearchTerms( search_queries_ids, search_queries, referer_hostnames_ids );                                     timer->stop("insertSearchQueries");
  timer->start("insertTvectorsInner");       lm.insertTrafficVectors( true, tvectors_inner_ids, tvectors_inner, referer_hostnames_ids, page_paths_full_ids);        timer->stop("insertTvectorsInner");
  timer->start("insertTvectorsIncoming");    lm.insertTrafficVectors( false, tvectors_incoming_ids, tvectors_incoming, referer_hostnames_ids, page_paths_full_ids); timer->stop("insertTvectorsIncoming");
  timer->start("insertHitsHourly");          lm.insertHitsPerHour( hits, real_did );                                                                                timer->stop("insertHitsHourly");
  timer->start("insertVisitsHourly");        lm.insertVisitsPerHour( visits, real_did, client_ips_ids );                                                            timer->stop("insertVisitsHourly");
  timer->start("insertPageviewsHourly");     lm.insertPageviewsPerHour( pageviews, real_did, client_ips_ids, page_paths_full_ids );                                 timer->stop("insertPageviewsHourly");
  timer->start("insertReferersPerHour");     lm.insertReferersPerHour( referers, real_did, page_paths_full_ids, referer_hostnames_ids );                            timer->stop("insertReferersPerHour");
  timer->start("insertSearchTermsPerHour");  lm.insertSearchTermsPerHour( search_terms, real_did, page_paths_full_ids, search_queries_ids, referer_hostnames_ids);  timer->stop("insertSearchTermsPerHour");
  timer->start("endThread");                 lm.endThread();                                                                                                        timer->stop("endThread"); 
}
std::map<unsigned long,int> HttpAccessLogMetrics::getClientIps(){
  return client_ips;
}
//PRIVATE
int HttpAccessLogMetrics::getLinesNumber(){
  int i =0;
  std::string line;
  std::ifstream statsFile(filename);
  while(std::getline(statsFile, line)){
    i++;
  }
  return i;
}
url_parts HttpAccessLogMetrics::getUrlParts( std::string url_string ){
  url_parts result;
  std::string proto,path_noproto,hostname,page_path,params_str;
  long long found = url_string.find("://");
  if(found !=-1){
    proto = url_string.substr(0,found);
    path_noproto = url_string.substr(found+3, url_string.length()-found);
  }
  else {
    path_noproto = "";
    hostname = "";
    page_path = "";
  }
  found = path_noproto.find("/");
  if(found == -1 ){
    hostname = path_noproto;
    page_path = "";
    result.full_path = "/";
  }
  else {
    hostname = path_noproto.substr(0,found);
    page_path = path_noproto.substr(found, path_noproto.length() - found );
    result.full_path = page_path;
  }
  found = page_path.find("?");
  if(found != -1){
    params_str = page_path.substr(found+1, page_path.length() - found);
    page_path = page_path.substr(0, found);
  }
  result.protocol = proto;
  result.hostname = hostname;
  result.params = params_str;
  result.path = page_path;
  return result;
}
url_parts HttpAccessLogMetrics::getUrlPartsFromReqPath( std::string requestPath, std::string proto, std::string hostname ){
  url_parts result;
  std::string page_path, params_str;
  result.protocol = proto;
  result.hostname = hostname;
  long long found = requestPath.find("?");
  if(found != -1){
    params_str = requestPath.substr(found+1, (requestPath.length()-found-1));
    page_path = requestPath.substr(0, found);
  }
  else{
    params_str = "";
    page_path = requestPath;
  }
  result.params = params_str;
  result.path = page_path;
  result.full_path = requestPath;
  return result;
}
//OTHER
void set_id_safely( int val ){
  std::lock_guard<std::mutex> lk(m);
  id=val;
}
void spawn_when_ready( int tid, int tpool_size, int tmax, int &ncompleted ) {
  std::string filename; 
  if( (long long) filenames.size() > tid){
    filename = dirname+"/"+filenames[tid];
    threads_timer->start(filenames[tid]);
  }
  std::string user_hostname = getHostnameFromLogfile(filename);
  std::cerr<<"thread "<<tid<<":"<<std::this_thread::get_id()<<" processing "<<filename<<std::endl;
  worker_threads_launched_total++;
  worker_threads_working++;
  HttpAccessLogMetrics hMetrics = HttpAccessLogMetrics( user_hostname, search_hosts, filename );
  if( hMetrics.getDomainId() != -1 ) {
    hMetrics.timer->start("logsScan"); hMetrics.logsScan( ); hMetrics.timer->stop("logsScan");
    hMetrics.insertEntities( );
    hMetrics.timer->printAllDurationsSorted();
  }
  ncompleted++;
  worker_threads_working--;
  std::thread(set_id_safely,id+1).join();
  std::string is_greater = (id < tmax) ? "<" : ">";
  std::cerr<<"ncompleted: "<<ncompleted<<"/"<<tmax <<" id: "<<id<<is_greater<<tmax <<" worker_threads_working: "<<worker_threads_working <<" worker_threads_launched_total: "<<worker_threads_launched_total<<std::endl; //" worker_threads_waiting: "<<worker_threads_waiting <<
  if( id < tmax  ) {
    std::thread( spawn_when_ready, id , tpool_size, tmax, std::ref(ncompleted)).detach();
  }
  if( (long long) filenames.size() > tid){
    threads_timer->stop(filenames[tid]);
  }
}
void start_thread_pool( int tpool_size ){
  std::thread( set_id_safely, 0 ).join();
  int ncompleted=0;
  int nt_total= filenames.size();
  for( int i = 0; i < tpool_size; i++){
    std::thread( spawn_when_ready, id, tpool_size, nt_total, std::ref(ncompleted) ).detach();
    std::thread( set_id_safely, id+1 ).join();
  }
  while( ncompleted < nt_total ) { // wait out in this thread till completion
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  threads_timer->printAllDurationsSorted();
}
