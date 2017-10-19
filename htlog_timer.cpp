#ifndef __HTLOG_TIMER__
#include "htlog_timer.hpp"
Timer::Timer(){ }
long long Timer::start( std::string timer_name ){
  start_times.insert({timer_name, std::chrono::steady_clock::now()});
  return (long long) std::chrono::duration_cast<std::chrono::milliseconds> ((start_times.find(timer_name)->second).time_since_epoch()).count();
}
long long Timer::stop( std::string timer_name ) {
  end_times.insert({timer_name, std::chrono::steady_clock::now()});
  long long duration = (long long) std::chrono::duration_cast<std::chrono::milliseconds>( (end_times.find(timer_name))->second -(start_times.find(timer_name))->second ).count();
  durations.insert({timer_name,duration});
  return duration;
}
long long Timer::getElapsedTimeMs( std::string timer_name ){
  long long duration=(durations.find(timer_name)->second);
  return duration;
}
void Timer::printAllDurations(){
  std::map<std::string, long long>::iterator it;
  for( it = durations.begin(); it != durations.end(); it++ ) {
    std::string timer_name = it->first;
    long long ms = it->second;
    std::cout<<timer_name<<" took "<<ms<<" ms."<<std::endl;
  }
}
void Timer::printAllDurationsSorted(){
  long long total_time_ms=0;
  sorted_durations.clear();
  for (std::map<std::string, long long>::iterator it = durations.begin(); it != durations.end(); it++) {
    total_time_ms+= it->second;
    sorted_durations.push_back(*it);
  }
  std::sort(sorted_durations.begin(), sorted_durations.end(), [=](std::pair<std::string, long long>& a, std::pair<std::string, long long>& b) {
    return a.second > b.second;
  });
  std::cout<<total_time_ms<<" ms TOTAL"<<std::endl;
  for(int i =0; i< (int)sorted_durations.size(); i++ ){
    std::pair<std::string, long long> p = sorted_durations[i];
    std::string timer_name = p.first;
    long long ms = p.second;
    std::cout<<ms<<" ms "<<timer_name<<std::endl;
  }
}
std::map<std::string, long long> Timer::getAllDurations(){
  return durations;
}
Timer::~Timer(){ }
#endif
