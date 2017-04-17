#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <locale.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#define _GNU_SOURCE
#include <search.h>
#include <sys/types.h>
#include "hashtab.h"
#include "ht_nodes.h"
#define ERROR_MAX          10240
#define LINE_MAX           1000000
#define CONFIG_DEBUG       1
#define CONFIG_STREAM_MODE 0
#define CONFIG_TIME_DELTA  0
#define HT_ALLOC_SIZE_MAX  1000000

int main(int argc, char **argv) {
  httpaccess_metrics *h_metrics;
  char *filename;
  int minargc=2;
  if( argc < minargc ) {
    printf("Usage:\n%s HTTPACCESS_LOG_FILENAME\n",argv[0]);
    fprintf(stderr, "ERROR: Insufficient args %d<%d\n",argc, minargc);
    exit(-1);
  }
  filename = argv[1];
  printf("logfile: %s\n", filename);
  setlocale(LC_ALL, "C");
  h_metrics = h_metrics_init();
  if ( logs_scan( h_metrics, filename ) ) {
    fprintf(stderr, "%s: %s\n", filename, h_metrics_get_error(h_metrics));
    exit(1);
  }
  //print_all_ips(ent);
  h_metrics_free(h_metrics);
  return 0;
}

