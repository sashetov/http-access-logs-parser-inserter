#ifndef __2D_STR_CONT__
typedef struct str_container {
  char * str;
} str_container_t;
str_container_t *** init_2d_str_cont(int x, int y);
void print_str_matrix( str_container_t *** matrix , int x, int y );
#define __2D_STR_CONT__
#endif
