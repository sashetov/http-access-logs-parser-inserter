DEBUG=-g
#CFLAGS=-O2 -Wall -W -fno-strict-aliasing -fno-omit-frame-pointer -v
CFLAGS=-Wall -W -fno-strict-aliasing -fno-omit-frame-pointer -v
PTHREAD_CFLAGS=-pthread
#CXXFLAGS= -std=c++0x -Wall -fPIC -O3 -fno-strict-aliasing -fno-omit-frame-pointer -v
CXXFLAGS=-std=c++0x -Wall -fPIC -fno-strict-aliasing -fno-omit-frame-pointer -v
export PKG_CONFIG_PATH='/opt/mysql-server/lib/pkgconfig:/usr/local/lib/pkgconfig'
MYSQLC_CFLAGS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags mysqlclient)
MYSQLC_LDFLAGS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs mysqlclient)
GEOIP_CFLAGS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags geoip)
GEOIP_LDFLAGS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs geoip)
UAP_LDFLAGS=-lboost_regex -lyaml-cpp
all: dump_cpp_vars clean test_progs cloudstats tags
cloudstats: clean_local htlog_urls.o uap.o htlog_uap.o hashtable.o htlog_processing.o linklist.o ht_nodes.o htlog_analyzer.o
	$(CXX) -o cloudstats htlog_urls.o htlog_analyzer.o htlog_processing.o ht_nodes.o hashtable.o linklist.o htlog_geoip.o htlog_mysql.o htlog_uap.o uap.o $(GEOIP_LDFLAGS) $(MYSQLC_LDFLAGS) $(UAP_LDFLAGS) $(PTHREAD_CFLAGS)
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
htlog_uap.o:
	$(CXX) -c $(CXXFLAGS) $(DEBUG) htlog_uap.c
uap.o:
	$(CXX) -c $(CXXFLAGS) $(DEBUG) uap.cpp
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
