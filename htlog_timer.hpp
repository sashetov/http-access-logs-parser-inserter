#ifndef __HTLOG_TIMER__
#include <time.h>
#include <ctime>
#include <chrono>
#include <map>
#include <iostream>
#include <vector>
#include <algorithm>
class Timer {
  public:
    Timer();
    long long start( std::string );
    long long stop( std::string );
    long long getElapsedTimeMs(std::string);
    std::map<std::string, long long> getAllDurations();
    void printAllDurations();
    void printAllDurationsSorted();
    ~Timer();
  private:
    std::map<std::string,std::chrono::steady_clock::time_point> start_times;
    std::map<std::string,std::chrono::steady_clock::time_point> end_times;
    std::map<std::string, long long> durations;
    std::vector<std::pair<std::string, long long>> sorted_durations;
};
#define __HTLOG_TIMER__
#endif
