#include <sys/types.h>
#include <stdio.h>
#ifndef __2D_STR_CONT__
#include "2dstrings.h"
#endif
str_container_t *** init_2d_str_cont(int x, int y){
  int i =0, j = 0,k =0;
  int container_size = (sizeof(str_container_t *));// + sizeof(int));
  str_container_t *** matrix = (str_container_t ***)malloc(x*y*container_size);
  for( i=0; i<x; i++ ) {
    matrix[i] = (str_container_t **)malloc(y*container_size);
    for(j=0; j<y; j++) {
      matrix[i][j] = (str_container_t *)malloc(container_size);
      matrix[i][j]->str = malloc( (i+j+1) * sizeof(char) );
      for(k=0; k < (i+j); k++) {
        matrix[i][j]->str[k] = 'a';
      }
      matrix[i][j]->str[k] = 0;
    }
  }
  return matrix;
}
void print_str_matrix( str_container_t *** matrix, int x, int y ){
  int i =0, j =0;
  for(i=0; i<x; i++) {
    for(j=0; j<y; j++) {
      printf("%d,%d: %s\n",i,j,matrix[i][j]->str);
    }
  }
}
int main() {
  str_container_t *** m =  init_2d_str_cont( 10, 10 );
  print_str_matrix(m, 10, 10);
  return 0;
}
