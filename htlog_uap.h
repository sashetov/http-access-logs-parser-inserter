#ifndef __HTLOG_UAP__
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#include <stdio.h>
#include <regex.h>
typedef struct str_container {
  char * str;
} str_container_t;
str_container_t *** get_regex_matches(
    char * str, char * regex_str, size_t num_groups, size_t max_matches );
str_container_t *** init_2d_str_cont(int x, int y, int max_str_size);

#ifdef __cplusplus
}
#endif
#endif
