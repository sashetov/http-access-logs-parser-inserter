#ifndef __HTLOG_UAP__
#ifdef __cplusplus
#ifndef __UAP__
#include "uap.hpp"
#endif
extern "C" {
#endif
#include <sys/types.h>
#include <stdio.h>
#include <regex.h>
#include <string.h>
#define MAX_STRING_SIZE 100
typedef struct str_container {
  char * str;
} str_container_t;
str_container_t *** get_regex_matches(
    char * str, char * regex_str, size_t num_groups, size_t max_matches );
ua_t * parse_user_agent( char * ua_str );
#ifdef __cplusplus
}
#endif
#endif
