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
#include <search.h>
#include <sys/types.h>
#include "htlog_processing.h"

int main(int argc, char **argv) {
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
  scan_file_to_loglines ( filename );
  return 0;
}
