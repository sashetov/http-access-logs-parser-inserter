/* create.c */
#include "pthread.h"
#include "sem.h"
void sem_create() {
  printf("sem_create\n");
  semaphore_t *semap;
  semap = semaphore_create("/tmp/semaphore");
  if (semap == NULL)
    exit(1);
  printf(" semap->count create:%d\n", semap->count);
  semaphore_close(semap);
}
void sem_post() {
  printf("sem_post\n");
  semaphore_t *semap;
  semap = semaphore_open("/tmp/semaphore");
  printf(" semap->count post open:%d\n", semap->count);
  if (semap == NULL) {
    exit(2);
  }
  semaphore_post(semap);
  printf(" semap->count after post:%d\n", semap->count);
  semaphore_close(semap);
}
void sem_wait(){
  printf("sem_wait\n");
  semaphore_t *semap;
  semap = semaphore_open("/tmp/semaphore");
  printf(" semap->count wait:%d\n", semap->count);
  if (semap == NULL) {
    exit(3);
  }
  semaphore_wait(semap);
  printf(" semap->count after wait:%d\n", semap->count);
  semaphore_close(semap);
}
int main() {
  int i = 0;
  pthread_t * func_thread = (pthread_t *)malloc( 3 * sizeof(pthread_t) );
  func_thread[0] = pthread_create( &func_thread[0], NULL, sem_create, (void*) NULL );
  func_thread[1] = pthread_create( &func_thread[2], NULL, sem_wait, (void*) NULL );
  func_thread[2] = pthread_create( &func_thread[1], NULL, sem_post, (void*) NULL );
  for( i= 0; i < 3; i++ ){
    pthread_join(func_thread[i], NULL);
  }
  free(func_thread);
  return (0);
}
