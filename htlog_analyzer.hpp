#ifndef __HTLOG_ANALYZER__
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <stdexcept>
struct SearchEngineContainer{
  std::string hostname;
  std::vector<std::string> query_params;
};
void print_usage_and_exit(std::ostream & stream, const int exit_status, std::string program_name );
void loadSearchEngines(std::vector<SearchEngineContainer> &, std::string);
std::vector<std::string> getLogfileNamesFromDirectory( std::string );
size_t getFilesize( std::string );
#define __HTLOG_ANALYZER__
#endif
