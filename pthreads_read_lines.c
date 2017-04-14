#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif
#define _POSIX_C_SOURCE 200809L
#include <time.h>
#include <errno.h>

int microsleep(long microsecs) {
  struct timespec req, rem;
  req.tv_sec = microsecs / 1000000;
  req.tv_nsec = (microsecs % 1000000) * 1000;
  int ret;
  while((ret = nanosleep(&req, &rem)) == -1 && errno == EINTR) req = rem;
  return ret;
}
void *print_tid( void *threadid ) {
  long tid;
  int i;
  #define MICROSECS 1000
  #define DOTS_NUM  1000
  tid = (long) threadid;
  printf("%ld", tid);
  for (i=0; i< DOTS_NUM; i++ ){
    microsleep(MICROSECS);
    printf(".");
    if(i == DOTS_NUM-1) {
      printf("%ld", tid);
    }
  }
  pthread_exit(NULL);
}
void read_n_lines_from_file(int tid, int n, char *filename){
  printf("void read_n_lines_from_file(int tid=%d, int n=%d, char *filename=%s)\n",(int)tid, (int)n , filename );
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  size_t read;
  int i;

  fp = fopen( filename , "r");
  if (fp == NULL) {
    printf("exit fail fopen\n");
    exit(EXIT_FAILURE);
  }

  i = 0;
  printf("tid=%d i=%d n=%d\n", (int)tid, (int)i, (int)n );

  while (((read = getline(&line, &len, fp)) != -1 ) && i < n ) {
    printf("tid=%d i=%d n=%d len=%d read=%zu line=%s\n",
       (int)tid, (int)i, (int)n, (int)len, read, line);
    i++;
  }
  
  fclose(fp);
  printf("fclose tid=%d\n", (int)tid );
  if (line) {
    free(line);
  }
}
struct func_params {
  long tid;
  char * filename;
  long num_lines;
};
void* func( void* params ){
  struct func_params *f_params = (struct func_params*) params;
  read_n_lines_from_file( (*f_params).tid, (*f_params).num_lines,
                          (*f_params).filename );
  free (params);
  return NULL;
}

void main() {
  // number of threads to launch
  long n = 40;
  long t, index = 0;
  int i,rc;
  struct func_params *params;
  pthread_t *func_thread = malloc( n * sizeof(pthread_t) );
  for( i= 0; i < n; i++ ){
    params = malloc(sizeof(struct func_params));
    (*params).tid = i;
    (*params).num_lines= (i+1)*10;
    (*params).filename= "/logdata/prgn-http-log-analyzer/dumps/dump.log";
    rc = pthread_create(&func_thread[i], NULL, func, (void*) params);
    if (rc){
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }
  for( i= 0; i < n; i++ ){
    pthread_join(func_thread[i], NULL);
  }
  free(func_thread);
}
