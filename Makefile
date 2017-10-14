export PKG_CONFIG_PATH='/opt/mysql-server/lib/pkgconfig:/usr/local/lib/pkgconfig'
DEBUG_FLAGS=-g -ggdb
WARNING_FLAGS=-Wall -Wextra -Werror -pedantic -v
SANITIZE_FLAGS=-L/usr/local/lib64/ -Wl,-R/usr/local/lib64/ -lasan -fsanitize=address -fsanitize=undefined -fsanitize=shift -fsanitize=integer-divide-by-zero -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=null -fsanitize=return -fsanitize=signed-integer-overflow #-fsanitize=thread 
LANGUAGE_FLAGS=-std=c++11 -fstrict-aliasing #-O3
STACKTRACE_FLAGS=-fno-omit-frame-pointer -fno-optimize-sibling-calls
CXXFLAGS=$(LANGUAGE_FLAGS) $(STACKTRACE_FLAGS) $(WARNING_FLAGS) $(DEBUG_FLAGS) $(SANITIZE_FLAGS)
PTHREAD_CFLAGS=-pthread
UAP_LDFLAGS=-lboost_regex -lyaml-cpp
GEOIP_LDFLAGS=-L/usr/local/lib/ -lgeolite2++ -lmaxminddb
MYSQL_CONN_LDFLAGS=-lmysqlcppconn
#MUDFLAP_FLAGS=-fmudflap -llibmudflap #export MUDFLAP_OPTIONS=-print-leaks;
all: dump_cpp_vars tags clean test_progs cloudstats cloudstats_test
test: clean cloudstats cloudstats_test
cloudstats: clean_local
	$(CXX) $(CXXFLAGS) $(PTHREAD_CFLAGS) htlog_mysql.cpp htlog_containers.cpp htlog_uap.cpp htlog_processing.cpp htlog_analyzer.cpp $(GEOIP_LDFLAGS) $(UAP_LDFLAGS) $(MYSQL_CONN_LDFLAGS) -o cloudstats
cloudstats_test:
	export ASAN_OPTIONS=symbolize=1; ASAN_SYMBOLIZER_PATH=$(which llvm-symbolizer); gdb -ex r -ex bt -args ./cloudstats logfiles
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
