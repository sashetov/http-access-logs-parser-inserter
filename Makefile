export PKG_CONFIG_PATH='/opt/mysql-server/lib/pkgconfig:/usr/local/lib/pkgconfig'
DEBUG=-g
CXXFLAGS=-O0 -std=c++11 -Wall -fno-strict-aliasing -fno-omit-frame-pointer -v # -fPIC 
PTHREAD_CFLAGS=-pthread
UAP_LDFLAGS=-lboost_regex -lyaml-cpp
GEOIP_LDFLAGS=-L /usr/local/lib/ -lgeolite2++ -lmaxminddb 
MYSQL_CONN_LDFLAGS=-lmysqlcppconn
all: dump_cpp_vars clean test_progs cloudstats tags
cloudstats: clean_local
	$(CXX) $(CXXFLAGS) $(DEBUG) $(PTHREAD_CFLAGS) htlog_mysql.cpp htlog_containers.cpp htlog_uap.cpp htlog_processing.cpp htlog_analyzer.cpp $(GEOIP_LDFLAGS) $(UAP_LDFLAGS) $(MYSQL_CONN_LDFLAGS) -o cloudstats
test_progs:
	$(MAKE) -C tests/ all
dump_cpp_vars:
	gcc -dM -E - < /dev/null
clean_local:
	rm -rf *.d *.o *.so *.a cloudstats logs/* core.*
tags:
	ctags --language-force=C *.h *.c
clean: clean_local
	$(MAKE) -C tests/ clean
