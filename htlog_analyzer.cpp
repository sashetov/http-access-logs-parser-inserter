#include "htlog_analyzer.hpp"
void print_usage_and_exit(std::ostream & stream, const int exit_status, std::string program_name ) {
 stream<<"Usage:\n"<<program_name<<" {OPTIONAL_OPTS} [REQUIRED_OPTS] OBJECT"<<std::endl
    <<" OPTIONAL_OPS:= { -s|--sql_logs_path SQL_LOGS_DIR "<<std::endl<<" }"<<std::endl
    <<"REQUIRED_OPTS:= [ -h|--mysql_host MYSQL_HOSTNAME "<<std::endl
    <<"                  -u|--mysql_user MYSQL_USER "<<std::endl
    <<"                  -P|--mysql_port MYSQL_PORT "<<std::endl
    <<"                  -p|--mysql_password MYSQL_PASSWORD "<<std::endl
    <<"                ]"<<std::endl
    <<"        OBJECT= LOGDIR_PATH"<<std::endl
    <<"NOTES:"<<std::endl
    <<"LOGDIR_PATH  - relative or full path to directory containing logfiles named DOMAIN.NAME.log and containing httpd access logs in apache combined format"<<std::endl
    <<"SQL_LOGS_DIR - relative or full path to directory where to dump logs of sql statements run for each domain name, each in its own log file"<<std::endl;
  exit(exit_status);
}
void loadSearchEngines( std::vector<SearchEngineContainer> &search_engines, std::string filename){
  std::string line;
  std::string se,qps,qp;
  int found;
  std::ifstream search_engines_file;
  //std::cerr<<"loading search engines data from "<<filename<<std::endl;
  search_engines_file.exceptions( std::ifstream::badbit | std::ifstream::failbit);
  try {
    search_engines_file.open(filename);
  }
  catch( const std::exception& e ){
    if(!search_engines_file.eof()){
      search_engines_file.close();
      std::cerr<<"loadSearchHostnames caught exception:\n"<<e.what()<<"\n";
      exit(1);
    }
  }
  try {
    while( !search_engines_file.eof() ){
      std::getline(search_engines_file, line);
      //std::cerr<<"line: "<<line<<std::endl;
      SearchEngineContainer sec;
      if ((found =(int) line.find_first_of(" ")) == -1){
        throw std::runtime_error("badly formatted search engine line:\n"+line);
      }
      se = line.substr(0,found);
      std::cerr<<"search engine hostname: "<<se<<std::endl;
      qps = line.substr( found+1, line.length()-found-1 );
      while ((found =(int) qps.find_first_of(" ")) != -1){
        qp = qps.substr( 0, found );
        sec.query_params.push_back( qp );
        std::cerr<<"query params string for "<<se<<": "<<qp<<std::endl;
        if( found+1 < (int)qps.length() ){
          qps = qps.substr( found + 1, line.length()-found-1 );
        }
      }
      sec.hostname= se;
      search_engines.push_back( sec );
    }
    search_engines_file.close();
  }
  catch( const std::exception& e ){
    if(!search_engines_file.eof()){
      search_engines_file.close();
      std::cerr<<"loadSearchHostnames caught exception:\n"<<e.what()<<"\n";
      exit(1);
    }
  }
}
std::vector<std::string> getLogfileNamesFromDirectory( std::string directory ){
  DIR *dir;
  struct dirent *entry;
  std::vector<std::string> result;
  std::string filename="";
  if((dir = opendir(directory.c_str())) == NULL) {
    std::cerr<< "Error(" << errno << ") opening " << directory << "\n";
    return result;
  }
  while((entry = readdir(dir)) != NULL){
    std::string filename = std::string( entry->d_name );
    if( filename =="." || filename == "..") {
      continue;
    }
    int filesize = (int) getFilesize(directory+"/"+filename);
    if( filesize > 0 ) {
      result.push_back(filename);
    }
  }
  closedir(dir);
  return result;
}
size_t getFilesize(const std::string filename) {
  struct stat st;
  if (stat(filename.c_str(), &st) < 0) {
    std::cerr<< "stat errno "<<errno<< std::endl;
    return 1;
  }
  return st.st_size;
}
