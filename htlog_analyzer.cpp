#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
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
  process_logfile( filename );
  return 0;
}
