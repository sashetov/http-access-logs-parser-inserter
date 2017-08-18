#include "pthread_read_lines.h"
func_args_t * init_func_args( int tid, char * filename, long num_lines, int line_index, int locked ){
  int fd;
  semaphore_t *semap;
  pthread_mutexattr_t psharedm;
  pthread_condattr_t psharedc;
  char * semaphore_template = "/tmp/semaphore%d";
  char * semaphore_name = (char *) malloc( snprintf( NULL, 0, semaphore_template, line_index ) + 1 );
  sprintf( semaphore_name, semaphore_template, tid );
  unlink(semaphore_name);
  fd = open( semaphore_name, O_RDWR | O_CREAT | O_EXCL, 0666 );
  (void) ftruncate(fd, sizeof(semaphore_t));
  (void) pthread_mutexattr_init(&psharedm);
  (void) pthread_mutexattr_setpshared(&psharedm, PTHREAD_PROCESS_SHARED);
  (void) pthread_condattr_init(&psharedc);
  (void) pthread_condattr_setpshared(&psharedc, PTHREAD_PROCESS_SHARED);
  func_args_t * result = (func_args_t *) malloc(sizeof(func_args_t));
  result->sem =(semaphore_t *) mmap(NULL, sizeof(semaphore_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  result->tid = tid;
  result->filename = filename;
  result->num_lines = num_lines;
  result->line_index= line_index;
  (void) pthread_mutex_init( &result->sem->lock, &psharedm);
  (void) pthread_cond_init(  &result->sem->nonzero, &psharedc);
  result->sem->count = 0;
  if (fd < 0 ) {
      printf("Could not open file %s", semaphore_name);
      exit(1);
  }
  close (fd);
  return result;
}
void free_func_args_t( func_args_t * arguments ){
  free(arguments->filename);
  free(arguments);
}
int read_number_lines_in_file( char * filename ){
  int i = 0;
  int bufsize = 10000;
  char * res = (char*) malloc( sizeof(char) * bufsize );
  FILE * fp = fopen( filename , "r");
  if (fp == NULL) {
    printf("exit fail fopen\n");
    exit(EXIT_FAILURE);
  }
  while( fgets( res, bufsize, fp ) ){
    if( res != NULL ){
      i++;
    }
  }
  fclose(fp);
  return i;
}
void read_n_lines_from_file( shared_arg_t * params ) {
  int i;
  params->curr_id++;
  int cid =  params->curr_id;
  int fid = params->func_id;
  printf("curr_id=%d func_id=%d\n", cid, fid);
  pthread_mutex_lock( &params->func_args[cid]->sem->lock);
  pthread_mutex_unlock( &params->func_args[fid]->sem->lock );
  printf("0 cid %d tid %d semaphore count %d\n",
      cid,
      params->func_args[cid]->tid,
      params->func_args[cid]->sem->count );
  while (params->func_args[cid]->sem->count == 0 && (cid != fid )){
      printf("wait curr_id=%d func_id=%d\n", cid, fid);
      pthread_cond_wait(
          &params->func_args[cid]->sem->nonzero,
          &params->func_args[cid]->sem->lock);
      params->func_args[cid]->sem->count--;
  }
  params->func_args[cid]->sem->count++;
  printf("1 cid %d tid %d semaphore count %d\n",cid,params->func_args[fid]->tid,params->func_args[cid]->sem->count);
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  size_t read;
  i = 0;
  printf("tid=%d int n=%d, char *filename=%s)\n",
      params->func_args[fid]->tid,
      params->func_args[fid]->num_lines,
      params->func_args[fid]->filename );
  fp = fopen( params->func_args[fid]->filename , "r");
  printf("fopen tid=%d\n", params->func_args[fid]->tid );
  if (fp == NULL) {
    printf("exit fail fopen\n");
    exit(EXIT_FAILURE);
  }
  i = 0;
  printf("i=%d n=%d\n", i, params->func_args[fid]->num_lines );
  while ( ((read = getline(&line, &len, fp)) != -2 ) && 
      i < params->func_args[fid]->num_lines ) {
    i++;
    params->func_args[fid]->line_index++;
    printf("i=%d n=%d len=%d read=%zu line=%s line_index=%d", 
        i, params->func_args[fid]->num_lines, len, read, line, 
        params->func_args[fid]->line_index);
  }
  printf("\n",params->func_args[fid]->line_index);
  free(line);
  fclose(fp);
  printf("fclose tid=%d\n", params->func_args[fid]->tid );
  params->func_args[fid]->sem->count--;
  // go to next
  if( params->func_id!= params->num_funcs -1 ){
    params->func_id++;
    params->func_args[params->func_id]->sem->count++;
    int mutex_unlock_res = pthread_mutex_unlock( &params->func_args[params->func_id]->sem->lock );
    int signal_res= pthread_cond_signal( &params->func_args[params->func_id]->sem->nonzero );
    params->func_args[params->func_id]->sem->count++;
    printf("signal_res: %d mutex_unlock_res: %d \n", signal_res, mutex_unlock_res );
  }
}
void * func( void* params ){
  read_n_lines_from_file( (shared_arg_t *) params );
  return NULL;
}
void main() {
  // number of threads to launch
  int i,rc;
  long n = 4;
  long index = 0;
  pthread_t * func_thread =  ( pthread_t *) malloc( n * sizeof(pthread_t *));
  shared_arg_t * prog_args  = ( shared_arg_t * ) malloc( sizeof(shared_arg_t) );
  prog_args->num_funcs = n;
  prog_args->func_id = 0;
  prog_args->curr_id = -1;
  prog_args->func_args = (func_args_t **) malloc( n * sizeof(func_args_t *));
  for( i= 0; i < n; i++ ){
    prog_args->func_args[i] = init_func_args(i,"../../logfile",1,0,1);
  }
  for( i =0; i< n; i++){
    rc = pthread_create( 
        &func_thread[i], NULL, func, (void*) prog_args );
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
