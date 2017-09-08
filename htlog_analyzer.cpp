#include <iostream>
#include "htlog_processing.hpp"

int main(int argc, char** argv) {
  //rm -f cloudstats; g++ -Wall -g -std=c++0x -pthread htlog_analyzer.cpp  -L /usr/local/lib/ -lgeolite2++ -lmaxminddb  -lboost_regex -lyaml-cpp -o cloudstats 2>&1 | grep -i error: ;rm -f logs/thread-*; ulimit -c unlimited; ./cloudstats logfile
  std::ios_base::sync_with_stdio(true);
  std::vector<std::string> user_hostnames;
  user_hostnames.push_back("atthematch.com");
  user_hostnames.push_back("www.atthematch.com");
  std::vector<std::string> search_hosts;
  search_hosts.push_back("google.com");
  search_hosts.push_back("google.co.uk");
  std::string filename;
  int minargc=2;
  if( argc < minargc ) {
    std::cout<<"Usage:\n"<< argv[0] <<" HTTPACCESS_LOG_FILENAME\n";
    fprintf(stderr, "ERROR: Insufficient args %d<%d\n",argc, minargc);
    exit(-1);
  }
  filename = std::string(argv[1]);
  std::cout<<"logfile: "<<filename<<"\n";
  HttpAccessLogMetrics hMetrics = HttpAccessLogMetrics(0,0,user_hostnames,search_hosts,filename);
  hMetrics.parseLogFile(40);
  return 0;
}
