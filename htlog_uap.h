#ifndef __HTLOG_UAP__
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#include <stdio.h>
#include <regex.h>
#define MAX_STRING_SIZE 100
typedef struct str_container {
  char * str;
} str_container_t;
typedef struct user_agent {
  char * client_ua_str;
  char * client_browser_vers;
  char * client_oses_vers;
  char * client_platform;
} user_agent_t;
str_container_t *** get_regex_matches(
    char * str, char * regex_str, size_t num_groups, size_t max_matches );
#ifdef __cplusplus
}
#endif
#endif
