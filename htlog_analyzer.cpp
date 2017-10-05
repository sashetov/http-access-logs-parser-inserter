#include <iostream>
#include <vector>
#include "htlog_processing.hpp"
std::string dirname;
std::vector<std::string> user_hostnames;
std::vector<std::string> search_hosts;
std::vector<std::string> filenames;
int main(int argc, char** argv) {
  std::ios_base::sync_with_stdio(true);
  int minargc=2;
  if( argc < minargc ) {
    std::cout<<"Usage:\n"<< argv[0] <<" LOGDIR_NAME/\n";
    fprintf(stderr, "ERROR: Insufficient args %d<%d\n",argc, minargc);
    exit(-1);
  }
  dirname= std::string(argv[1]);
  std::cout<<"directory name : "<<dirname<<"\n";
  std::string user_hostname = getHostnameFromLogfile(dirname);
  user_hostnames.push_back(user_hostname);
  user_hostnames.push_back("www."+user_hostname);
  loadSearchHostnames(search_hosts,"search_engines");
  filenames = getLogfileNamesFromDirectory(dirname);
  if( filenames.size() == 0){
    std::cout<<"No non-empty files found in directory"<<"\n";
  }
  std::cout<<"filenames in dir "<<filenames.size()<<"\n";
  //parseNLogfilesAtATime(20, dirname, filenames, user_hostnames, search_hosts );
  start_thread_pool(20);
  return 0;
}
