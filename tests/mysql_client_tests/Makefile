DEBUG=-g
CFLAGS=-O2 -Wall -W -fno-strict-aliasing -fno-omit-frame-pointer
export PKG_CONFIG_PATH='/opt/mysql-server/lib/pkgconfig'
MYSQLC_CFLAGS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags mysqlclient)
MYSQLC_LDFLAGS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs mysqlclient)

all: clean mysql_progs
mysql_progs: mysql_query_did mysql_version
mysql_version.o: mysql_version.c
	$(CC) -c mysql_version.c $(CFLAGS) $(MYSQLC_CFLAGS) $(DEBUG)
mysql_version: mysql_version.o
	$(CC) -o mysql_version mysql_version.o $(MYSQLC_LDFLAGS)

mysql_query_did.o: mysql_query_did.c
	$(CC) -c mysql_query_did.c $(CFLAGS) $(MYSQLC_CFLAGS) $(DEBUG)
mysql_query_did: mysql_query_did.o
	$(CC) -o mysql_query_did mysql_query_did.o $(MYSQLC_LDFLAGS)

clean:
	rm -rf *.o mysql_query_did mysql_version
