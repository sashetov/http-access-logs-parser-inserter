CXXFLAGS=-std=c++11 -Wall -g -fno-strict-aliasing -fno-omit-frame-pointer -v
PTHREAD_LDFLAGS=-lpthread
all: clean test
worker-pool: 
	$(CXX) -std=c++11 -Wall -g worker-pool.cpp -o worker-pool $(PTHREAD_LDFLAGS)
test: worker-pool
	./test.sh
clean:
	find . -name "*.o" -exec rm -rf {} \;
	rm -f worker-pool *.a *.so
