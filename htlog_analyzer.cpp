#include <iostream>
#include <vector>
#include "htlog_processing.hpp"
#include <sys/resource.h>
#include <getopt.h>
std::string dirname;
std::string logfile;
std::vector<std::string> search_hosts;
std::vector<std::string> filenames;
int main(int argc, char** argv) {
  int c;
  while (1) {
    int option_index = 0;
    static struct option long_options[] = { {"logfile", 1, 0,  0 } };
    c = getopt_long(argc, argv, "l:", long_options, &option_index);
    if (c == -1) {
      break;
    }
    std::string option_value = "";
    switch (c) {
      case 0:
        if(std::string(long_options[option_index].name) == "logfile" && optarg ){
          logfile = std::string(optarg);
        }
        break;
      case 'l':
        logfile = std::string(optarg);
        break;
      default:
        std::cerr<<"invalid option '"<<c<<"' provided"<<std::endl;
    }
  }
  if(logfile.size()){
    std::cout<<"using "<<logfile<<" for output\n";
  }
  if (optind < argc) {
    dirname = std::string(argv[optind++]);
    std::cout<<"Analyzing logfiles in directory: "<<dirname<<"\n";
  }
  else {
    std::cerr<<"Usage:\n"<< argv[0] <<" { -l ERROR.LOG | --logfile ERROR.LOG } LOGDIR_NAME/\n";
    exit(-1);
  }
  loadSearchHostnames(search_hosts,"search_engines");
  filenames = getLogfileNamesFromDirectory(dirname);
  if( filenames.size() == 0){
    std::cout<<"No non-empty files found in directory"<<"\n";
  }
  std::cout<<"filenames in dir "<<filenames.size()<<"\n";
  int num_threads = std::thread::hardware_concurrency();
  Timer * main_timer = new Timer();
  main_timer->start("main");
  start_thread_pool(num_threads);
  main_timer->stop("main");
  main_timer->printAllDurationsSorted();
  return 0;
}
