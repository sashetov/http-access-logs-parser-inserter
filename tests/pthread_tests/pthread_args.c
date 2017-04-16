#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct BoundedBuffer {
  pthread_t readThread;
  pthread_mutex_t mutexBuffer;
} buffer2;

struct readThreadParams {
  struct BoundedBuffer b;
  char* data;
  int count;
};

void *bufferRead (void *context) {
  struct readThreadParams *readParams = context;
  pthread_mutex_lock(&readParams->b.mutexBuffer);
  pthread_mutex_unlock(&readParams->b.mutexBuffer);
  return NULL;
}

int main(void) {
  int ret;
  char *out_array = malloc(42);
  size_t in_size = 42;
  struct readThreadParams readParams;
  readParams.b = buffer2;
  readParams.data = out_array;
  readParams.count = in_size;
  ret = pthread_create(&readParams.b.readThread, NULL, bufferRead, &readParams);
  printf("%d\n",ret);
  if (!ret) {
    pthread_join(&readParams.b.readThread, NULL);
  }
  free(out_array);
  return ret;
}



