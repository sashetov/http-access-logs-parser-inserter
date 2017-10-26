CXXFLAGS=-std=c++11 -Wall -g -fno-strict-aliasing -fno-omit-frame-pointer -v
PTHREAD_LDFLAGS=-lpthread
THREAD_POOL_SIZE=20
TOTAL_THREADS_TO_LAUNCH=10000
all: clean test
worker-pool: 
	$(CXX) -std=c++11 -Wall -g worker-pool.cpp -o worker-pool $(PTHREAD_LDFLAGS)
test: worker-pool
	 ./worker-pool $(THREAD_POOL_SIZE) $(TOTAL_THREADS_TO_LAUNCH)
clean:
	find . -name "*.o" -exec rm -rf {} \;
	rm -f worker-pool *.a *.so
