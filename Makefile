export PKG_CONFIG_PATH='/opt/mysql-server/lib/pkgconfig:/usr/local/lib/pkgconfig'
DEBUG=-g
CFLAGS=-Wall -W -fno-strict-aliasing -fno-omit-frame-pointer -v# -O2 #4gdb
CXXFLAGS=-std=c++0x -Wall -fPIC -fno-strict-aliasing -fno-omit-frame-pointer -v # -O3 #4gdb
MYSQLC_CFLAGS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags mysqlclient)
GEOIP_CFLAGS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags geoip)
PTHREAD_CFLAGS=-pthread
MYSQLC_LDFLAGS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs mysqlclient)
GEOIP_LDFLAGS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs geoip)
PTHREAD_LDFLAGS=-lpthread
UAP_LDFLAGS=-lboost_regex -lyaml-cpp
all: dump_cpp_vars clean test_progs cloudstats tags
cloudstats: clean_local htlog_urls.o uap.o htlog_uap.o hashtable.o htlog_processing.o linklist.o ht_nodes.o htlog_analyzer.o
	$(CXX) -o cloudstats htlog_urls.o htlog_analyzer.o htlog_processing.o ht_nodes.o hashtable.o linklist.o htlog_geoip.o htlog_mysql.o htlog_uap.o uap.o $(GEOIP_LDFLAGS) $(MYSQLC_LDFLAGS) $(UAP_LDFLAGS) $(PTHREAD_LDFLAGS)
htlog_analyzer.o:
	$(CXX) -c $(CXXFLAGS) $(DEBUG) $(MYSQLC_CFLAGS) htlog_analyzer.cpp
htlog_processing.o:
	$(CXX) -c $(CXXFLAGS) $(PTHREAD_CFLAGS) $(GEOIP_CFLAGS) $(MYSQLC_CFLAGS) $(DEBUG) htlog_geoip.c htlog_mysql.c htlog_processing.c
linklist.o:
	$(CC) -c $(CFLAGS) $(DEBUG) linklist.c
ht_nodes.o:
	$(CC) -c $(CFLAGS) $(DEBUG) ht_nodes.c
hashtable.o:
	$(CC) -c $(CFLAGS) $(DEBUG) hashtable.c
uap.o:
	$(CXX) -c $(CXXFLAGS) $(DEBUG) uap.cpp
htlog_uap.o:
	$(CC) -c $(CFLAGS) $(DEBUG) htlog_uap.c
htlog_urls.o:
	$(CC) -c $(CFLAGS) $(DEBUG) htlog_urls.c
test_progs:
	$(MAKE) -C tests/ all
dump_cpp_vars:
	gcc -dM -E - < /dev/null
clean_local:
	rm -rf *.d *.o htlog_analyzer cloudstats 
tags:
	ctags --language-force=C *.h *.c
clean: clean_local
	$(MAKE) -C tests/ clean
