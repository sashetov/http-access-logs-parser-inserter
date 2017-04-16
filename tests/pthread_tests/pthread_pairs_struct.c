#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif
#define _POSIX_C_SOURCE 200809L
#include <time.h>
#include <errno.h>

struct Pairs {
  long i,j;
};
/*void* Compare(void* pair){
  struct Pairs *my_pair = (struct Pairs*)pair;
  printf("\nThread %ld, %ld", (*my_pair).i, (*my_pair).j);
  return NULL;
}*/
void* Compare(void* pair){
  struct Pairs *my_pair = (struct Pairs*)pair;
  printf("\nThread %ld, %ld", (*my_pair).i, (*my_pair).j);
  free (pair);
  return NULL;
}
void main() {
  long n = 12;
  long thread_cmp_count = (long)n*(n-1)/2;
  long t,index = 0;
  int thread;
  struct Pairs *pair;
  pthread_t *cmp_thread = malloc(thread_cmp_count*sizeof(pthread_t));
  for(thread = 0;(thread < thread_cmp_count); thread++){
    for(t = thread+1; t < n; t++){
      // allocate a separate pair for each thread
      pair = malloc(sizeof(struct Pairs));
      (*pair).i = thread;
      (*pair).j = t;
      pthread_create(&cmp_thread[index++], NULL, Compare, (void*) pair);
    }
  }

  for(thread= 0;(thread<thread_cmp_count); thread++){
    pthread_join(cmp_thread[thread], NULL);
  }

  free(cmp_thread);

  /*pair = malloc(sizeof(struct Pairs));
  pthread_t * cmp_thread = malloc(thread_cmp_count*sizeof(pthread_t));
  for( i = 0; i < thread_cmp_count; i++){
    for( t = i + 1; t < n; t++){
      (*pair).i = i;
      (*pair).j = t;
      pthread_create(&cmp_thread[index++], NULL, Compare, (void*) pair);
    }
  }
  for( i= 0; i< thread_cmp_count; i++ ){
    pthread_join(cmp_thread[i], NULL);
  }
  free(cmp_thread);*/
}
