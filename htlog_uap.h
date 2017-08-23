#ifndef __HTLOG_UAP__
#ifdef __cplusplus
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
str_container_t *** get_regex_matches( char * str, char * regex_str, size_t num_groups, size_t max_matches );
char * get_version_string( char * major, char * minor, char * patch, char * patch_minor );
typedef struct ua_agent {
  char * family;
  char * major;
  char * minor;
  char * patch;
  char * patch_minor;
} ua_agent_t;
typedef struct ua_device {
  char * family;
  char * model;
  char * brand;
} ua_device_t;
typedef struct ua {
  ua_device_t * device;
  ua_agent_t * os;
  ua_agent_t * browser;
} ua_t;
void free_c_ua( ua_t * user_agent );
#define __HTLOG_UAP__
#ifdef __cplusplus
}
#endif
#endif
