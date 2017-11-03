#include <stdio.h>
#include <thread>
#include <mutex>
#include <functional>
#include <iostream>
#include <vector>
#include <condition_variable>
#include <chrono>
std::mutex m_id, m_n;
void inc_id( int &id ){
  std::lock_guard<std::mutex> lk_id(m_id, std::adopt_lock);
  id++;
}
void inc_n( int &n ){
  std::lock_guard<std::mutex> lk_n(m_n,  std::adopt_lock);
  n++;
}
void spawn_if_ready( int ttotal, int &id, int &n ) {
  std::lock(m_id, m_n);
  std::lock_guard<std::mutex> lk_id(m_id, std::adopt_lock);
  std::lock_guard<std::mutex> lk_n(m_n, std::adopt_lock);
  if(id < ttotal){
    std::thread( inc_id, std::ref(id) ).join();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    std::thread( inc_n, std::ref(n) ).join();
    std::lock_guard<std::mutex> lk_n(m_n, std::adopt_lock);
    if(n < ttotal) {
      std::thread( spawn_if_ready, ttotal, std::ref(id), std::ref(n) ).join();
    }
  }
}
void start_thread_pool( int tpool_size, int ttotal, int &id, int& n ){
  int i;
  for(i=0; i< tpool_size; i++){
    std::thread( spawn_if_ready, ttotal, std::ref(id), std::ref(n) ).detach();
  }
  while( n < ttotal ){
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  std::cout<<id<<" "<<n<<std::endl;
}
int main(int argc, char** argv) {
  int id=0, n=0;
  int minargc =2;
  if( argc < minargc ) {
    std::cout<<"Usage:\n"<< argv[0] <<" THREAD_POOL_SIZE TOTAL_THREADS_TO_LAUNCH\n";
    fprintf(stderr, "ERROR: Insufficient args %d<%d\n",argc, minargc);
    exit(-1);
  }
  int thread_pool_size = std::stoi(std::string(argv[1]));
  int ttotal = std::stoi(std::string(argv[2]));
  std::cerr<<"thread_pool_size "<<thread_pool_size<<" total_threads "<<ttotal<<std::endl;
  start_thread_pool( thread_pool_size, ttotal, std::ref(id), std::ref(n) );
}
