#include <iostream>
#include <vector>
#include "htlog_processing.hpp"
#include <sys/resource.h>
std::string dirname;
std::vector<std::string> search_hosts;
std::vector<std::string> filenames;
void reset_rlimit_stack(){
  const rlim_t k_stack_size = 512 * 1024 * 1024;
  struct rlimit rl;
  int result;
  result = getrlimit(RLIMIT_STACK, &rl);
  if (result == 0) {
    if (rl.rlim_cur < k_stack_size ) {
      rl.rlim_cur = k_stack_size;
      result = setrlimit(RLIMIT_STACK, &rl);
      if (result != 0) {
        fprintf(stderr, "setrlimit returned result = %d\n", result);
      }
    }
  }
}
int main(int argc, char** argv) {
  int minargc=2;
  if( argc < minargc ) {
    std::cout<<"Usage:\n"<< argv[0] <<" LOGDIR_NAME/\n";
    fprintf(stderr, "ERROR: Insufficient args %d<%d\n",argc, minargc);
    exit(-1);
  }
  //reset_rlimit_stack();
  //std::ios_base::sync_with_stdio(true);
  dirname= std::string(argv[1]);
  std::cout<<"directory name : "<<dirname<<"\n";
  loadSearchHostnames(search_hosts,"search_engines");
  filenames = getLogfileNamesFromDirectory(dirname);
  if( filenames.size() == 0){
    std::cout<<"No non-empty files found in directory"<<"\n";
  }
  std::cout<<"filenames in dir "<<filenames.size()<<"\n";
  int num_threads = 10; std::thread::hardware_concurrency();
  start_thread_pool(num_threads);
  return 0;
}
