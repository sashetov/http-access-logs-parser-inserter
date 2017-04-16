DEBUG=-g
CFLAGS=-O2 -Wall -W -fno-strict-aliasing -fno-omit-frame-pointer
MYSQLC_CFLAGS=`pkg-config --cflags mysqlclient`
MYSQLC_LDFLAGS=`pkg-config --libs mysqlclient`
PTHREAD_CFLAGS=-pthread

all: clean mysql_progs pthread_progs stats_progs

mysql_progs: mysql_query_did mysql_version
mysql_version.o: mysql_version.c
	$(CC) -c mysql_version.c $(CFLAGS) $(MYSQLC_CFLAGS) $(DEBUG)
mysql_version: mysql_version.o
	$(CC) -o mysql_version mysql_version.o $(MYSQLC_LDFLAGS)

mysql_query_did.o: mysql_query_did.c
	$(CC) -c mysql_query_did.c $(CFLAGS) $(MYSQLC_CFLAGS) $(DEBUG)
mysql_query_did: mysql_query_did.o
	$(CC) -o mysql_query_did mysql_query_did.o $(MYSQLC_LDFLAGS)

pthread_progs: pthread_read_n_files pthreads_read_lines
pthread_read_n_files: pthreads_read_n_files.c
	$(CC) -o pthread_read_n_files pthreads_read_n_files.c $(CFLAGS) $(PTHREAD_CFLAGS) $(DEBUG)

pthreads_read_lines: pthreads_read_lines.c
	$(CC) -o pthreads_read_lines pthreads_read_lines.c $(CFLAGS) $(PTHREAD_CFLAGS) $(DEBUG)

stats_progs: parser
parse.o: parse.c
	$(CC) -c parse.c $(CFLAGS) $(DEBUG)
parser: parse.o
	$(CC) -o parser parse.o

clean:
	rm -rf *.o mysql_query_did mysql_version pthreads_read_lines pthread_read_n_files parser 
