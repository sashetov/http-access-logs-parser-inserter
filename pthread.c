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

void *print_tid(void *threadid/*, int n*/)
{
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


void *launch_threads( int num_threads, void *function ){
  pthread_t threads[num_threads];
  int rc;
  long t;
  int n=0;
  for( t=0; t < num_threads; t++ ) {
    rc = pthread_create(&threads[t], NULL, function, (void *)t);
    n++;
    if (rc){
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }

}
int main(int argc, char *argv[])
{
  launch_threads(900, print_tid);
  pthread_exit(NULL);
}
