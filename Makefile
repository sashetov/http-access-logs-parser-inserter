DEBUG=-g
#CFLAGS=-O2 -Wall -W -fno-strict-aliasing -fno-omit-frame-pointer -v
CFLAGS= -Wall -W -fno-strict-aliasing -fno-omit-frame-pointer -v
export PKG_CONFIG_PATH='/opt/mysql-server/lib/pkgconfig:/usr/local/lib/pkgconfig'
MYSQLC_CFLAGS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags mysqlclient)
MYSQLC_LDFLAGS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs mysqlclient)
GEOIP_CFLAGS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags geoip)
GEOIP_LDFLAGS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs geoip)

all: dump_cpp_vars clean test_progs cloudstats tags
cloudstats: clean_local htlog_analyzer.o
	$(CC) -o cloudstats linklist.o hashtable.o ht_nodes.o htlog_processing.o htlog_uap.o htlog_geoip.o htlog_mysql.o htlog_analyzer.o $(MYSQLC_LDFLAGS) $(GEOIP_LDFLAGS)
htlog_analyzer.o:
	$(CC) -c $(CFLAGS) $(MYSQLC_CFLAGS) $(GEOIP_CFLAGS) $(DEBUG)  linklist.c hashtable.c ht_nodes.c htlog_processing.c htlog_uap.c htlog_geoip.c htlog_mysql.c htlog_analyzer.c
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
