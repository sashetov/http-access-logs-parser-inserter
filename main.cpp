#include "htlog_analyzer.hpp"
#include "htlog_processing.hpp"
#include <sys/resource.h>
#include <getopt.h>
std::string mysql_hostname, mysql_port_num, mysql_user, mysql_password, dirname, sql_logs_path;
std::vector<SearchEngineContainer> search_hosts;
std::vector<std::string> filenames;
int main(int argc, char** argv) {
  int optchar, exit_status=0;
  std::string program_name = std::string(argv[0],strlen(argv[0]));
  while (1) {
    int option_index = 0;
    static struct option long_options[] = {
      {"sql_logs_path", 1, 0,  0 },
      {"mysql_host", 1, 0,  0 },
      {"mysql_port", 1, 0,  0 },
      {"mysql_user", 1, 0,  0 },
      {"mysql_password", 1, 0,  0 },
    };
    optchar = getopt_long(argc, argv, "s:h:u:P:p:g", long_options, &option_index);
    if (optchar == -1) {
      break;
    }
    std::string option_value = "";
    switch (optchar) {
      case 0:
        if(std::string(long_options[option_index].name) == "sql_logs_path" && optarg ){
          sql_logs_path = std::string(optarg);
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
        else if(std::string(long_options[option_index].name) == "usage" ){
          print_usage_and_exit( exit_status == 0 ? std::cout : std::cerr, exit_status, program_name );
        }
        break;
      case 's':
        sql_logs_path = std::string(optarg);
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
      case 'g':
        print_usage_and_exit( exit_status == 0 ? std::cout : std::cerr, exit_status, program_name );
        break;
      default:
        std::cerr<<"invalid option '"<<optchar<<"' provided"<<std::endl;
        break;
    }
  }
  if (optind < argc) {
    dirname = std::string(argv[optind++]);
    std::cout<<"Analyzing logfiles in directory: "<<dirname<<"\n";
  }
  if( mysql_hostname.size() ==0) {
    std::cerr<<"Error: MYSQL_HOSTNAME (-h | --mysql_host ) option is required! "<<std::endl;
    exit_status++;
  }
  if( mysql_port_num.size()==0 ){
    std::cerr<<"Error: MYSQL_PORT option ( -P | --mysql_port ) is required! "<<std::endl;
    exit_status++;
  }
  if( mysql_user.size() == 0 ){
    std::cerr<<"Error: MYSQL_USER (-u | --mysql_user ) option is required! "<<std::endl;
    exit_status++;
  }
  if( mysql_password.size()==0 ){
    std::cerr<<"Error: MYSQL_PASSWORD (-p | --mysql_password )option is required! "<<std::endl;
    exit_status++;
  }
  if ( exit_status ){
    print_usage_and_exit( std::cerr, exit_status, program_name );
  }
  loadSearchEngines( search_hosts, "search_engines" );
  filenames = getLogfileNamesFromDirectory(dirname);
  if( filenames.size() == 0) {
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
