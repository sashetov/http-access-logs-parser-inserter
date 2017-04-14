#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif
#define _POSIX_C_SOURCE 200809L
#include <time.h>
#include <errno.h>

struct func_params {
  long i,j;
};
void* func( void* params ){
  struct func_params *f_params = (struct func_params*) params;
  printf("\nThread %ld, %ld", (*f_params).i, (*f_params).j);
  free (params);
  return NULL;
}
void main() {
  long n = 12;
  long t,index = 0;
  int i;
  struct func_params *params;
  pthread_t *func_thread = malloc( n * sizeof(pthread_t) );
  for(i= 0;i<n; i++){
    params = malloc(sizeof(struct func_params));
    (*params).i = i;
    (*params).j = i+1;
    pthread_create(&func_thread[i], NULL, func, (void*) params);
  }
  for( i= 0; i < n; i++ ){
    pthread_join(func_thread[i], NULL);
  }
  free(func_thread);
}
