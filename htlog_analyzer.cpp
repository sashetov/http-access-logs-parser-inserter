#include <iostream>
#include <vector>
#include "htlog_processing.hpp"

int main(int argc, char** argv) {
  std::ios_base::sync_with_stdio(true);
  std::vector<std::string> user_hostnames;
  user_hostnames.push_back("atthematch.com");
  user_hostnames.push_back("www.atthematch.com");
  std::vector<std::string> search_hosts;
  search_hosts.push_back("www.google.com");
  search_hosts.push_back("google.com");
  search_hosts.push_back("google.co.uk");
  search_hosts.push_back("www.google.co.uk");
  search_hosts.push_back("google.co.in");
  search_hosts.push_back("www.google.co.in");
  search_hosts.push_back("bing.com");
  search_hosts.push_back("www.bing.com");
  std::string filename;
  int minargc=2;
  if( argc < minargc ) {
    std::cout<<"Usage:\n"<< argv[0] <<" HTTPACCESS_LOG_FILENAME\n";
    fprintf(stderr, "ERROR: Insufficient args %d<%d\n",argc, minargc);
    exit(-1);
  }
  filename = std::string(argv[1]);
  std::cout<<"logfile: "<<filename<<"\n";
  HttpAccessLogMetrics hMetrics = HttpAccessLogMetrics(user_hostnames,search_hosts,filename);
  hMetrics.parseLogFile(4);
  hMetrics.insertEntities();
  hMetrics.printAllIdsMaps();
  return 0;
}
