#include "htlog_processing.hpp"
//HALM
HttpAccessLogMetrics::HttpAccessLogMetrics( int did, int user_id, std::vector<std::string> user_hosts, std::vector<std::string> search_hosts, std::string file ){
  st = time(NULL);
  real_did = did;
  uid = user_id;
  int i =0;
  lines_failed=0;
  lines_processed=0;
  for( i =0; i< (int)user_hosts.size(); i++){
    internal_hostnames.push_back( user_hosts[i]);
  }
  for( i =0; i< (int)search_hosts.size(); i++){
    search_hostnames.push_back( search_hosts[i]);
  }
  filename = file;
}
HttpAccessLogMetrics::~HttpAccessLogMetrics(){
}
int HttpAccessLogMetrics::logsScanParallel( int threadNumber, int linesNumber, long startLine ){
  std::cout<<"threadNumber "<<threadNumber<<" linesNumber "<<linesNumber<<" startLine "<<startLine<<"\n";
  std::ifstream statsFile;
  int i = 0;
  std::string line;
  try {
    statsFile.exceptions( std::ifstream::badbit | std::ifstream::failbit);
    statsFile.open(filename);
    // don't use [] operator to check for \0 - segfaults for some reason..
    if (filename == "-") {
      return 1;
    }
    while (std::getline(statsFile, line) && i < startLine ) { 
      i++;
    }
    i=0;
    while (!statsFile.eof() && i < linesNumber){
      parsed_logline ll;
      std::getline(statsFile, line);
      if (parseLine(line,ll) == 0) {
        incrementCount( &client_ips, ll.userIP);
        incrementCount( &client_geo_locations, getCountryFromIP(ll.userIPStr));
        processUserAgent( ll.agent );
        processRefererStrings( ll.referrer );
        processRequestUrl(ll.requestURL);
        lines_processed++;
      }
      else {
        lines_failed++;
        std::cerr<<"Invalid line: "<< line<<"\n";
      }
      i++;
    }
    statsFile.close();
    //threadLogFile.close();
  } catch (const std::exception& e) {
    if(!statsFile.eof()){
      std::cout<<"caught exception: "<<e.what()<<"\n";
    }
    return 6;
  }
  et = time(NULL);
  return 0;
}
int HttpAccessLogMetrics::getLinesNumber(){
  int i =0;
  std::string line;
  std::ifstream statsFile(filename);
  while(std::getline(statsFile, line)){
    i++;
  }
  return i;
}
void HttpAccessLogMetrics::parseLogFile( int numThreads ){
  int i =0;
  int lines = getLinesNumber();
  int remainder = lines % numThreads;
  int chunkLines = lines / numThreads;
  long lineIndex = 0;
  int threadNumber = 0;
  for( i= 0; i < numThreads ; i++ ){
    int numLines = chunkLines;
    if( i == numThreads -1 ){
      numLines += remainder;
    }
    threads.push_back( new std::thread( 
          &HttpAccessLogMetrics::logsScanParallel, this,
          threadNumber, numLines, lineIndex ));
    lineIndex += numLines;
    threadNumber++;
  }
  for( i =0; i< numThreads; i++){
    threads[i]->join();
  }
  std::cout<<"client_ips "<<client_ips.size()<<"\n";
  std::map<unsigned long,int>::iterator ul_it;
  for( ul_it = client_ips.begin(); ul_it!=client_ips.end(); ul_it++){
      unsigned long ip= ul_it->first;
      int count = ul_it->second;
      std::cout<<"ip: "<<getStringIP(ip)<<" count: "<<count<<"\n";
  }
  std::cout<<"client_geo_locations "<<client_geo_locations.size()<<"\n";
  std::map<std::string,int>::iterator str_it;
  for( str_it = client_geo_locations.begin(); str_it!=client_geo_locations.end(); str_it++){
      std::string key = str_it->first;
      int count = str_it->second;
      std::cout<<"location : "<<key<<" count: "<<count<<"\n";
  }
  std::cout<<"client_devices "<<client_devices.size()<<"\n";
  for( str_it = client_devices.begin(); str_it!=client_devices.end(); str_it++){
      std::string key = str_it->first;
      int count = str_it->second;
      std::cout<<"client_devices: "<<key<<" count: "<<count<<"\n";
  }
  std::cout<<"client_oses "<<client_oses.size()<<"\n";
  for( str_it = client_oses.begin(); str_it!=client_oses.end(); str_it++){
      std::string key = str_it->first;
      int count = str_it->second;
      std::cout<<"client_oses: "<<key<<" count: "<<count<<"\n";
  }
  std::cout<<"client_browsers "<<client_browsers.size()<<"\n";
  for( str_it = client_browsers.begin(); str_it!=client_browsers.end(); str_it++){
      std::string key = str_it->first;
      int count = str_it->second;
      std::cout<<"client_browsers: "<<key<<" count: "<<count<<"\n";
  }
  std::cout<<"page_paths "<<page_paths.size()<<"\n";
  for( str_it = page_paths.begin(); str_it!=page_paths.end(); str_it++){
      std::string key = str_it->first;
      int count = str_it->second;
      std::cout<<"page_paths: "<<key<<" count: "<<count<<"\n";
  }
  std::cout<<"referer_hostnames "<<referer_hostnames.size()<<"\n";
  for( str_it = referer_hostnames.begin(); str_it!=referer_hostnames.end(); str_it++){
      std::string key = str_it->first;
      int count = str_it->second;
      std::cout<<"referer_hostnames: "<<key<<" count: "<<count<<"\n";
  }
  std::cout<<"referer_pathstrings "<<referer_pathstrings.size()<<"\n";
  for( str_it = referer_pathstrings.begin(); str_it!=referer_pathstrings.end(); str_it++){
      std::string key = str_it->first;
      int count = str_it->second;
      std::cout<<"referer_pathstrings: "<<key<<" count: "<<count<<"\n";
  }
  std::cout<<"referer_params "<<referer_params.size()<<"\n";
  std::map<ParamsContainer,int>::iterator pc_it;
  for( pc_it = referer_params.begin(); pc_it!=referer_params.end(); pc_it++){
      ParamsContainer key = pc_it->first;
      int count = pc_it->second;
      std::cout<<key.getPageType()<<" referer_hostname "<<key.getHost()<<" referer_page: "<<key.getPage()<<" referer param: "<<key.getKey()<<" param_value: "<<key.getValue()<<" count: "<<count<<"\n";
  }
  std::cout<<"internref_hostnames "<<internal_hostnames.size()<<"\n";

  std::cout<<"internref_pathstrings "<<internref_pathstrings.size()<<"\n";

  std::cout<<"internref_params "<<internref_params.size()<<"\n";
  for( pc_it = internref_params.begin(); pc_it!=internref_params.end(); pc_it++){
      ParamsContainer key = pc_it->first;
      int count = pc_it->second;
      std::cout<<key.getPageType()<<" internref_hostname "<<key.getHost()<<" internref_page: "<<key.getPage()<<" internref param: "<<key.getKey()<<" param_value: "<<key.getValue()<<" count: "<<count<<"\n";
  }
  std::cout<<"search_queries "<<search_queries.size()<<"\n";
  std::map<KeyValueContainer,int>::iterator kv_it;
  for( kv_it = search_queries.begin(); kv_it!=search_queries.end(); kv_it++){
      KeyValueContainer key = kv_it->first;
      int count = kv_it->second;
      std::cout<<"query: "<<key.getKey()<<" search engine:"<<key.getValue()<<" count: "<<count<<"\n";
  }
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
unsigned long HttpAccessLogMetrics::getTimestamp( std::string dateTime ){
  struct tm tm;
  time_t epoch;
  if ( strptime(dateTime.c_str(), "%d/%h/%Y:%H:%M:%S %Z", &tm) != NULL ){
    epoch = mktime(&tm);
  }
  else {
    return -1;
  }
  return (unsigned long) epoch;
}
int HttpAccessLogMetrics::parseLine( std::string line, parsed_logline &ll ){
  //char *user_hostname, *date, *hour, *timezone, *host, *agent, *req, *ref, *p;
  std::string host, userIP, 
    date, hour, timezone,
    request, requestType, requestURL, requestProtocol,
    referrer,
    userAgent,
    size,
    status;
  int statusCode;
  long sizeBytes;
  int uIPCursor0, uIPCursor1,
      dateCursor0, dateCursor1,
      reqCursor0, reqCursor1,
      reqUrlCursor0, reqUrlCursor1,
      refCursor0, refCursor1,
      uaCursor0, uaCursor1,
      statusCursor0, statusCursor1,
      sizeCursor0, sizeCursor1
        ;
  //char *agent_start = NULL, *req_end = NULL, *ref_end = NULL;
  int agentIndex = 0, reqEnd = 0, refEnd = 0, agentWithoutParentheses= 0;
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
  requestURL = request.substr(reqUrlCursor0+1, reqUrlCursor1 -reqUrlCursor0 -1);
  requestType = request.substr(0,reqUrlCursor0);
  requestProtocol= request.substr(reqUrlCursor1+1, request.size()-reqUrlCursor1-1);
  //REFERRER
  if ((refCursor0= line.find_first_of("\"", reqCursor1+1)) == -1){
    return 9;
  }
  if ((refCursor1= line.find_first_of("\"",refCursor0+1)) == -1){
    return 10;
  }
  referrer= line.substr( refCursor0+1, refCursor1-refCursor0-1 );
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
  ll.requestURL = requestURL;
  ll.requestType = requestType;
  ll.referrer = referrer;
  ll.agent = userAgent;
  ll.sizeBytes = sizeBytes;
  ll.statusCode= statusCode;
  /*std::cout
    <<"hostname:"<<host<<"\n"
    <<"userIP:"<<userIP<<"\n"
    <<"date:"<<date<<"\n"
    <<"request:"<<request<<"\n"
    <<"requestURL:"<<requestURL<<"\n"
    <<"requestType:"<<requestType<<"\n"
    <<"requestProtocol:"<<requestProtocol<<"\n"
    <<"referrer:"<<referrer<<"\n"
    <<"userAgent:"<<userAgent<<"\n"
    <<"statusCode:"<<statusCode<<"\n"
    <<"sizeBytes:"<<sizeBytes<<"\n"
    ;*/
  /*std::cout
    <<"ll->hostname:"<<ll.hostname<<"\n"
    <<"userIP:"<<userIP<<"\n"
    <<"ll->userIP:"<<ll.userIP<<"\n"
    <<"ll->date:"<<ll.date<<"\n"
    <<"ll->timestamp:"<<ll.timestamp<<"\n"
    <<"ll->requestURL:"<<ll.requestURL<<"\n"
    <<"ll->requestType:"<<ll.requestType<<"\n"
    <<"ll->referrer:"<<ll.referrer<<"\n"
    <<"ll->agent:"<<ll.agent<<"\n"
    <<"ll->sizeBytes:"<<ll.sizeBytes<<"\n"
    <<"ll->statusCode:"<<ll.statusCode<<"\n"
    ;*/
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
  incrementCount( &client_devices, (std::string) ua.device.toString() );
  incrementCount( &client_oses, (std::string)ua.os.toString() );
  incrementCount( &client_browsers, (std::string)ua.browser.toString() );
}
std::vector<KeyValueContainer> HttpAccessLogMetrics::parseParamsString( std::string params_str ){
  std::vector<KeyValueContainer> results;
  std::size_t found;
  if( params_str.length() > 0 ) { 
    std::string workstring(params_str);
    std::string param;
    while((found = workstring.find("&") )!= -1 ){
      std::string query;
      param = workstring.substr(0,found);
      workstring = workstring.substr(found+1, workstring.length()-found-1);
      if((found = param.find("=")) != -1 ){
        std::string key = param.substr(0,found);
        std::string val = param.substr(found+1, param.length()-found-1);
        results.push_back(KeyValueContainer(key,val));
      }
    }
    param = workstring;
    if((found = param.find("=")) != -1 ){
      std::string key = param.substr(0,found);
      std::string val = param.substr(found+1, param.length()-found-1);
      results.push_back(KeyValueContainer(key,val));
    }
  }
  return results;
}
std::vector<ParamsContainer> HttpAccessLogMetrics::parseParamsString( std::string params_str, int type, std::string hostname, std::string page_path ){
  std::vector<ParamsContainer> results;
  std::size_t found;
  if( params_str.length() > 0 ) { 
    std::string workstring(params_str);
    std::string param;
    while((found = workstring.find("&") )!= -1 ){
      std::string query;
      param = workstring.substr(0,found);
      workstring = workstring.substr(found+1, workstring.length()-found-1);
      if((found = param.find("=")) != -1 ){
        std::string key = param.substr(0,found);
        std::string val = param.substr(found+1, param.length()-found-1);
        results.push_back(ParamsContainer(type,hostname,page_path,key,val));
      }
    }
    param = workstring;
    if((found = param.find("=")) != -1 ){
      std::string key = param.substr(0,found);
      std::string val = param.substr(found+1, param.length()-found-1);
      results.push_back(ParamsContainer(type,hostname,page_path,key,val));
    }
  }
  return results;
}
url_parts HttpAccessLogMetrics::getUrlParts( std::string url_string ){
  url_parts result;
  std::string proto,path_noproto,hostname,page_path,params_str;
  std::size_t found = url_string.find("://");
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
  }
  else {
    hostname = path_noproto.substr(0,found);
    page_path = path_noproto.substr(found+1, path_noproto.length() - found );
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
url_parts HttpAccessLogMetrics::getUrlPartsFromReqURL( std::string requestURL, std::string proto, std::string hostname ){
  url_parts result;
  std::string page_path, params_str;
  result.protocol = proto;
  result.hostname = hostname;
  std::size_t found = requestURL.find("?");
  if(found != -1){
    params_str = requestURL.substr(found+1, (requestURL.length()-found-1));
    page_path = requestURL.substr(0, found);
  }
  else{
    params_str = "";
    page_path = requestURL;
  }
  result.params = params_str;
  result.path = page_path;
  return result;
}
void HttpAccessLogMetrics::processRefererStrings( std::string referer ){
  std::vector<std::string>::iterator it;
  std::vector<ParamsContainer> params;
  std::vector<ParamsContainer>::iterator params_it;
  int hostname_type= 0; // 0 nothing found, 1 found internal ref, 2 found external ref, 3 found search ref
  url_parts url_parsed = getUrlParts( referer );
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
  for (  it = search_hostnames.begin(); it != search_hostnames.end(); it++ ) {
    if( url_parsed.hostname == *it ){
      hostname_type = 3;
      break;
    }
  }
  if(hostname_type == 1 ) {
    if( url_parsed.hostname.length() > 0 ) {
      //std::cout<<"internref_hostname '"<<url_parsed.hostname<<"'\n";
      incrementCount(&internref_hostnames, url_parsed.hostname);
    }
    if( url_parsed.path.length() > 0 ) {
      //std::cout<<"internref_pathstring: '"<<url_parsed.path<<"'\n";
      incrementCount(&internref_pathstrings, url_parsed.path );
    }
    if( url_parsed.params.length() > 0) {
      if( url_parsed.path.length() > 0 && url_parsed.hostname.length() > 0 ) {
        params = parseParamsString( url_parsed.params, hostname_type, url_parsed.hostname, url_parsed.path);
      }
      //std::cout<<"internref_paramstring: '"<<url_parsed.params<<"'\n";
      for (  params_it = params.begin() ; params_it != params.end(); params_it++){
        //std::cout<<"internref_param key '"<<(*params_it).getKey()<<"'\n";
        //std::cout<<"internref_param value '"<<(*params_it).getValue()<<"'\n";
        incrementCount(&internref_params, (*params_it));
      }
    }
  }
  else if(hostname_type == 2 ) {
    if( url_parsed.hostname.length() > 0 ) {
      //std::cout<<"referer_hostname '"<<url_parsed.hostname<<"'\n";
      incrementCount(&referer_hostnames, url_parsed.hostname);
    }
    if( url_parsed.path.length() > 0 ) {
      //std::cout<<"referer_pathstring: '"<<url_parsed.path<<"'\n";
      incrementCount(&referer_pathstrings, url_parsed.path);
    }
    if( url_parsed.params.length() > 0) {
      if( url_parsed.path.length() > 0 && url_parsed.hostname.length() > 0 ) {
        params = parseParamsString( url_parsed.params, hostname_type, url_parsed.hostname, url_parsed.path);
      }
      //std::cout<<"referer_paramstring: '"<<url_parsed.params<<"'\n";
      for( params_it = params.begin(); params_it!=params.end(); params_it++){
        //std::cout<<"referer_param key '"<<(*params_it).getKey()<<"'\n";
        //std::cout<<"referer_param value '"<<(*params_it).getValue()<<"'\n";
        incrementCount(&referer_params, (*params_it));
      }
    }
  }
  else if(hostname_type == 3 ) {
    if( url_parsed.params.length() > 0 && url_parsed.path.length() > 0 && url_parsed.hostname.length() > 0 ) {
      params = parseParamsString( url_parsed.params, hostname_type, url_parsed.hostname, url_parsed.path);
    }
    for( params_it = params.begin(); params_it!=params.end(); params_it++){
      if((*params_it).getKey() == "q"){
        std::cout<<"search_terms_param key '"<<(*params_it).getKey()<<"'\n";
        std::cout<<"search_terms_param value '"<<(*params_it).getValue()<<"'\n";
        KeyValueContainer searchContainer((*params_it).getValue(),(*params_it).getHost());
        incrementCount(&search_queries, searchContainer);
      }
    }
  }
}
void HttpAccessLogMetrics::processRequestUrl( std::string requestURL ){
  std::vector<ParamsContainer> params;
  std::vector<ParamsContainer>::iterator params_it;
  url_parts url_parsed = getUrlPartsFromReqURL( requestURL, "http", internal_hostnames[0] );
  int hostname_type = 1;
  if( url_parsed.path.length()> 0){
    incrementCount(&page_paths, url_parsed.path);
    incrementCount(&internref_pathstrings, url_parsed.path);
  }
  if( url_parsed.hostname.length()>0){
    incrementCount(&internref_hostnames, url_parsed.hostname);
  }
  if(url_parsed.params.length() > 0 ) {
    if( url_parsed.path.length() > 0 && url_parsed.hostname.length() > 0 ) {
      params = parseParamsString( url_parsed.params, hostname_type, url_parsed.hostname, url_parsed.path);
    }
    for( params_it = params.begin(); params_it!=params.end(); params_it++){
      incrementCount(&internref_params, (*params_it));
    }
  }
}
