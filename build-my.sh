#!/bin/bash

export PKG_CONFIG_PATH='/opt/mysql-server/lib/pkgconfig';
echo '/opt/mysql-server/lib/' > /etc/ld.so.conf.d/mysql-client.conf
ldconfig
ldconfig  -p | grep libmysql
echo gcc -c `pkg-config --cflags mysqlclient` mysql_version.c
gcc -c `pkg-config --cflags mysqlclient` mysql_version.c
echo gcc -o mysql_version mysql_version.o `pkg-config --libs mysqlclient`
gcc -o mysql_version mysql_version.o `pkg-config --libs mysqlclient`

#echo gcc mysql_version.c -o mysql_version `mysql_config --cflags --libs`
#gcc mysql_version.c -o mysql_version `mysql_config --cflags --libs`
