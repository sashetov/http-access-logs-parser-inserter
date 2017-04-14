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

void *read_n_lines_from_file(int n, char *filename){
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  int i;

  fp = fopen( filename , "r");
  if (fp == NULL) {
    exit(EXIT_FAILURE);
  }
  i = 0;
  while ((read = getline(&line, &len, fp)) != -1 && i<n) {
    printf("Retrieved line of length %zu :\n", read);
    printf("%s", line);
    i++;
  }
  fclose(fp);
  if (line) {
    free(line);
  }
  exit(EXIT_SUCCESS);
}

void *launch_threads( int num_threads, void *start_routine, void *arg ){
  pthread_t threads[num_threads];
  int rc;
  long i;
  int n=0;
  for( i=0; i < num_threads; i++ ) {
    rc = pthread_create( &threads[i], NULL, start_routine, arg );
    n++;
    if (rc) {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }
}

int main(int argc, char *argv[])
{
  launch_threads(10, launch_threads, "/logdata/prgn-http-log-analyzer/dumps/dump.log");
  pthread_exit(NULL);
}
