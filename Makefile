DEBUG=-g
CFLAGS=-O2 -Wall -W -fno-strict-aliasing -fno-omit-frame-pointer -v
CXXFLAGS= -Wall -O3 -fno-strict-aliasing -fno-omit-frame-pointer -v
export PKG_CONFIG_PATH='/opt/mysql-server/lib/pkgconfig:/usr/local/lib/pkgconfig'
MYSQLC_CFLAGS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags mysqlclient)
MYSQLC_LDFLAGS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs mysqlclient)
GEOIP_CFLAGS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags geoip)
GEOIP_LDFLAGS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs geoip)

all: dump_cpp_vars clean test_progs cloudstats tags
cloudstats: clean_local  htlog_geoip.o htlog_uap.o hashtable.o htlog_processing.o linklist.o ht_nodes.o htlog_analyzer.o
	$(CXX) -o cloudstats htlog_geoip.o htlog_mysql.o linklist.o hashtable.o htlog_processing.o ht_nodes.o htlog_uap.o htlog_analyzer.o $(GEOIP_LDFLAGS) $(MYSQLC_LDFLAGS)
htlog_analyzer.o:
	$(CXX) -c $(CXXFLAGS) $(DEBUG) htlog_analyzer.cpp $(MYSQLC_CFLAGS)
htlog_uap.o:
	$(CC) -c htlog_uap.c $(CFLAGS) $(DEBUG)
linklist.o:
	$(CC) -c linklist.c $(CFLAGS) $(DEBUG)
htlog_processing.o:
	$(CC) -c htlog_processing.c htlog_mysql.c $(CFLAGS) $(MYSQLC_CFLAGS) $(DEBUG)
ht_nodes.o:
	$(CC) -c ht_nodes.c $(CFLAGS) $(DEBUG)
hashtable.o:
	$(CC) -c hashtable.c $(CFLAGS) $(DEBUG)
htlog_geoip.o:
	$(CC) -c htlog_geoip.c $(CFLAGS) $(GEOIP_CFLAGS) $(DEBUG)
test_progs:
	$(MAKE) -C tests/ all
dump_cpp_vars:
	gcc -dM -E - < /dev/null
clean_local:
	rm -rf *.o htlog_analyzer cloudstats 
tags:
	ctags --language-force=C *.h *.c
clean: clean_local
	$(MAKE) -C tests/ clean
