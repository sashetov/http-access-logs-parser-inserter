export PKG_CONFIG_PATH='/opt/mysql-server/lib/pkgconfig:/usr/local/lib/pkgconfig'
LANGUAGE_FLAGS=-std=c++11 -fstrict-aliasing
STACKTRACE_FLAGS=-fno-omit-frame-pointer -fno-optimize-sibling-calls
WARNING_FLAGS=-Wall -Wextra -Werror -pedantic -v
OPTIMIZE_FLAGS=-O3
DEBUG_FLAGS=-g -ggdb3
CXXFLAGS=$(LANGUAGE_FLAGS) $(STACKTRACE_FLAGS) $(WARNING_FLAGS) $(OPTIMIZE_FLAGS) $(DEBUG_FLAGS)
PTHREAD_FLAGS=-pthread
SANITIZE_UNDEF_FLAGS=-fsanitize=undefined -fsanitize=shift -fsanitize=integer-divide-by-zero -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=null -fsanitize=return -fsanitize=signed-integer-overflow
SANITIZE_ADDR_FLAGS=-fsanitize=address
SANITIZE_ADDR_LDFLAGS=-L/usr/local/lib64/ -Wl,-R/usr/local/lib64/ -lasan
GEOIP_LDFLAGS=-L/usr/local/lib/ -lgeolite2++ -lmaxminddb
UAP_LDFLAGS=-lboost_regex -lyaml-cpp
MYSQL_CONN_LDFLAGS=-lmysqlcppconn
all: dump_cpp_vars tags clean test_progs cloudstats cloudstats_test
test: clean cloudstats cloudstats_test
cloudstats: clean_local
	$(CXX) $(CXXFLAGS) $(PTHREAD_FLAGS) $(SANITIZE_UNDEF_FLAGS) $(SANITIZE_ADDR_FLAGS) htlog_timer.cpp htlog_mysql.cpp htlog_containers.cpp htlog_uap.cpp htlog_processing.cpp htlog_analyzer.cpp $(SANITIZE_ADDR_LDFLAGS) $(GEOIP_LDFLAGS) $(UAP_LDFLAGS) $(MYSQL_CONN_LDFLAGS) -o cloudstats
	./tools/transfer.sh
cloudstats_test:
	./tools/cloudstats-test.sh
test_progs:
	$(MAKE) -C tests/ all
dump_cpp_vars:
	gcc -dM -E - < /dev/null
clean_local:
	rm -rf *.d *.o *.so *.a cloudstats logs/* core.*
tags:
	ctags --language-force=c++ *.h *.cpp *.hpp
clean: clean_local
	$(MAKE) -C tests/ clean
