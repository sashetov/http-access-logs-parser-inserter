#include <stdio.h>
#include <thread>
#include <iostream>
#include <vector>
#include <condition_variable>
#include <chrono>
std::vector<int> lock_signal_vars;
std::vector<std::thread> worker_threads;
std::vector<std::thread> signaling_threads;
std::condition_variable cv;
std::mutex m;
void notify( int tid ) {
  std::lock_guard<std::mutex> lk(m);
  std::cerr << "notifying "<<tid<<"\n";
  lock_signal_vars[tid] = tid;
  cv.notify_all();
}
void spawn_when_ready( int tid, int tpool_size, int tmax, int &ncompleted ) {
  std::unique_lock<std::mutex> lk(m);
  cv.wait( lk, [=] { // c++11 for lambda [](){}
      std::cerr << "waiting on lsv["<<tid<<"]=="<<tid<<"\n";
      return lock_signal_vars[tid] == tid;
      });
  std::cout<<"."<<std::endl;
  ncompleted++;
  std::cerr <<"condition reached lsv["<<tid<<"]=="<< tid <<" ncompleted: "<<ncompleted<<"\n";
  //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  int new_tid = tid + tpool_size;//worker_threads.size();
  if( new_tid < tmax ){
    lock_signal_vars[new_tid] = -1;
    worker_threads[new_tid]= std::thread( spawn_when_ready, new_tid, tpool_size, tmax, std::ref(ncompleted));
    signaling_threads[new_tid]= std::thread( notify, new_tid );
    worker_threads[new_tid].detach();
    signaling_threads[new_tid].detach();
  }
}
void start_thread_pool( int tpool_size, int nt_total ){
  int i, completed=0;
  lock_signal_vars.resize(nt_total);
  worker_threads.resize(nt_total);
  signaling_threads.resize(nt_total);
  for(i=0; i< tpool_size; i++){
    lock_signal_vars[i] = -1;
    worker_threads[i] = std::thread( spawn_when_ready, i, tpool_size, nt_total, std::ref(completed));
    signaling_threads[i] = std::thread( notify, i );
    worker_threads[i].join();
    signaling_threads[i].join();
  }
  while(completed< nt_total ){
  }
}
int main(int argc, char** argv) {
  int minargc =2;
  if( argc < minargc ) {
    std::cout<<"Usage:\n"<< argv[0] <<" THREAD_POOL_SIZE TOTAL_THREADS_TO_LAUNCH\n";
    fprintf(stderr, "ERROR: Insufficient args %d<%d\n",argc, minargc);
    exit(-1);
  }
  int thread_pool_size = std::stoi(std::string(argv[1]));
  int total_threads = std::stoi(std::string(argv[2]));
  std::cerr<<"thread_pool_size "<<thread_pool_size
    <<" total_threads "<<total_threads<<"\n";
  start_thread_pool( thread_pool_size, total_threads );
}
