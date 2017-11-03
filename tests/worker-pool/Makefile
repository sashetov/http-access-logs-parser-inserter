SANITIZE_ADDR_FLAGS=-fsanitize=address
CXXFLAGS=-std=c++11 -Wall -g -fno-strict-aliasing -fno-omit-frame-pointer -v $(SANITIZE_ADDR_FLAGS)
PTHREAD_LDFLAGS=-lpthread
SANITIZE_ADDR_LDFLAGS=-L/usr/local/lib64/ -Wl,-R/usr/local/lib64/ -lasan
all: clean test
worker-pool: 
	$(CXX) $(CXXFLAGS) worker-pool.cpp -o worker-pool $(PTHREAD_LDFLAGS) $(SANITIZE_ADDR_LDFLAGS)

test: worker-pool
	./test.sh
clean:
	find . -name "*.o" -exec rm -rf {} \;
	rm -f worker-pool *.a *.so
