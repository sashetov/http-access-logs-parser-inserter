export PKG_CONFIG_PATH='/opt/mysql-server/lib/pkgconfig:/usr/local/lib/pkgconfig:/usr/lib64/pkgconfig/'
LANGUAGE_FLAGS=-std=c++11 -fstrict-aliasing
FORMAT_FLAGS=-fmessage-length=0 -fdiagnostics-color=always
STACKTRACE_FLAGS=-fno-omit-frame-pointer -fno-optimize-sibling-calls
WARNING_FLAGS=-Wall -Wextra -Werror -pedantic -v
OPTIMIZE_FLAGS_PROD=-O3
OPTIMIZE_FLAGS_DEV=-Og
COMPILE_FLAGS_PROD=-pipe $(OPTIMIZE_FLAGS_PROD) -march=native
COMPILE_FLAGS_DEV=-pipe $(OPTIMIZE_FLAGS_DEV) -march=native
DEBUG_FLAGS=-g -ggdb3
PTHREAD_FLAGS=-pthread
SANITIZE_UNDEF_FLAGS=-fsanitize=undefined -fsanitize=shift -fsanitize=integer-divide-by-zero -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=null -fsanitize=return -fsanitize=signed-integer-overflow
SANITIZE_ADDR_FLAGS=-fsanitize=address
CXXFLAGS=$(LANGUAGE_FLAGS) $(FORMAT_FLAGS) $(STACKTRACE_FLAGS) $(WARNING_FLAGS) $(COMPILE_FLAGS_DEV) $(DEBUG_FLAGS) $(PTHREAD_FLAGS) $(SANITIZE_UNDEF_FLAGS) $(SANITIZE_ADDR_FLAGS) 
SANITIZE_ADDR_LDFLAGS=-L/usr/local/lib64/ -Wl,-R/usr/local/lib64/ -lasan
GEOIP_LDFLAGS=`pkg-config --libs --cflags libmaxminddb`
UAP_LDFLAGS=-lboost_regex -lyaml-cpp
MYSQL_CPP_CONN_LDFLAGS=-L/usr/local/lib -lmysqlcppconn
INCLUDEPATH=-I`pwd`/ -I/usr/local/include/cppconn/ -I/usr/local/include/
LDFLAGS=$(SANITIZE_ADDR_LDFLAGS) $(GEOIP_LDFLAGS) $(UAP_LDFLAGS) $(MYSQL_CPP_CONN_LDFLAGS)
all: dump_cpp_vars tags clean test_progs test
readme:
	{ echo '```'; ./cloudstats -g; echo '```'; }> README.md
clean_local:
	rm -rf *.d *.o *.so *.a cloudstats logs/* core.*
tags:
	ctags --language-force=c++ *.h *.cpp *.hpp
cloudstats: clean_local tags
	$(CXX) $(INCLUDEPATH) $(CXXFLAGS) *.cpp -o cloudstats $(LDFLAGS)
transfer:
	./tools/transfer.sh
cloudstats_and_transfer: cloudstats readme transfer
cloudstats_test: cloudstats
	./tools/cloudstats-test.sh
test_progs:
	$(MAKE) -C tests/ all
dump_cpp_vars:
	gcc -dM -E - < /dev/null
clean: clean_local
	$(MAKE) -C tests/ clean
test: clean cloudstats_and_transfer cloudstats_test
