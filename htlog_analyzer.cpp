#include <iostream>
#include <vector>
#include "htlog_processing.hpp"
#include <sys/resource.h>
#include <getopt.h>
std::string mysql_hostname, mysql_port_num, mysql_user, mysql_password, dirname, log_path;
std::vector<std::string> search_hosts;
std::vector<std::string> filenames;
int main(int argc, char** argv) {
  int c;
  while (1) {
    int option_index = 0;
    static struct option long_options[] = {
      {"log_path", 1, 0,  0 },
      {"mysql_host", 1, 0,  0 },
      {"mysql_port", 1, 0,  0 },
      {"mysql_user", 1, 0,  0 },
      {"mysql_password", 1, 0,  0 },
    };
    c = getopt_long(argc, argv, "l:h:u:P:p:", long_options, &option_index);
    if (c == -1) {
      break;
    }
    std::string option_value = "";
    switch (c) {
      case 0:
        if(std::string(long_options[option_index].name) == "log_path" && optarg ){
          log_path = std::string(optarg);
        }
        else if(std::string(long_options[option_index].name) == "mysql_host" && optarg ){
          mysql_hostname = std::string(optarg);
        }
        else if(std::string(long_options[option_index].name) == "mysql_port" && optarg ){
          mysql_port_num = std::string(optarg);
        }
        else if(std::string(long_options[option_index].name) == "mysql_user" && optarg ){
          mysql_user = std::string(optarg);
        }
        else if(std::string(long_options[option_index].name) == "mysql_password" && optarg ){
          mysql_password = std::string(optarg);
        }
        break;
      case 'l':
        log_path = std::string(optarg);
        break;
      case 'h':
        mysql_hostname = std::string(optarg);
        break;
      case 'u':
        mysql_user = std::string(optarg);
        break;
      case 'P':
        mysql_port_num = std::string(optarg);
        break;
      case 'p':
        mysql_password = std::string(optarg);
        break;
      default:
        std::cerr<<"invalid option '"<<c<<"' provided"<<std::endl;
    }
  }
  if (optind < argc) {
    dirname = std::string(argv[optind++]);
    std::cout<<"Analyzing logfiles in directory: "<<dirname<<"\n";
  }
  int print_usage_and_exit=0;
  if( mysql_hostname.size() ==0) {
    std::cerr<<"Error: MYSQL_HOSTNAME (-h or --mysql_host ) option is required! "<<std::endl;
    print_usage_and_exit++;
  }
  if( mysql_port_num.size()==0 ){
    std::cerr<<"Error: MYSQL_PORT option ( -P or --mysql_port ) is required! "<<std::endl;
    print_usage_and_exit++;
  }
  if( mysql_user.size() == 0 ){
    std::cerr<<"Error: MYSQL_USER (-u or --mysql_user ) option is required! "<<std::endl;
    print_usage_and_exit++;
  }
  if( mysql_password.size()==0 ){
    std::cerr<<"Error: MYSQL_PASSWORD (-p or --mysql_password )option is required! "<<std::endl;
    print_usage_and_exit++;
  }
  if ( print_usage_and_exit){ 
    std::cerr<<"Usage:\n"<< argv[0] <<" { -l ERROR.LOG | --log_path ERROR.LOG } -h MYSQL_HOSTNAME -u MYSQL_USER -P MYSQL_PORT -p MYSQL_PASSWORD LOGDIR_NAME/\n";
    exit(print_usage_and_exit);
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
