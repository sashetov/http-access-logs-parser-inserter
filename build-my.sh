#!/bin/bash

export PKG_CONFIG_PATH='/opt/mysql-server/lib/pkgconfig';
echo '/opt/mysql-server/lib/' > /etc/ld.so.conf.d/mysql-client.conf
ldconfig
#ldconfig  -p | grep libmysql
#gcc -c `pkg-config --cflags mysqlclient` mysql_version.c
#gcc -o mysql_version mysql_version.o `pkg-config --libs mysqlclient`
gcc -c `pkg-config --cflags mysqlclient` mysql_query_did.c
gcc -o mysql_query_did mysql_query_did.o `pkg-config --libs mysqlclient`

