#ifdef __cplusplus
extern "C" {
#endif
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif
#define _POSIX_C_SOURCE 200809L
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
struct semaphore {
  pthread_mutex_t lock;
  pthread_cond_t nonzero;
  unsigned count;
};
typedef struct semaphore semaphore_t;
typedef struct func_args {
  char * filename;
  long num_lines;
  int line_index;
  int tid;
  int threads_locked;
  //pthread_cond_t wait_cond;
  //pthread_mutex_t lock;
  semaphore_t * sem;
} func_args_t;
typedef struct shared_arg {
  int func_id;
  int curr_id;
  int num_funcs;
  func_args_t **  func_args;
} shared_arg_t;
func_args_t * init_func_args( int tid, char * filename, long num_lines, int line_index, int locked );
void print_shared_arg( shared_arg_t );
void free_func_args_t( func_args_t * args );
void read_n_lines_from_file( shared_arg_t * params);
void* func( void* params );
#ifdef __cplusplus
}
#endif
