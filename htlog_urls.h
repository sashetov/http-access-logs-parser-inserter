#ifndef __HTLOG_URLS__
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#include <stdio.h>
#include <regex.h>
#include <string.h>
typedef struct url_params {
  char * key;
  char * value;
} url_params_t;
typedef struct referer_url {
  int   is_internal;
  char * path_str;
  char * params_str;
  url_params_t * params;
} referer_url_t;
char * get_referer_url( char * referer_str, char ** internal_hostnames );
char * get_search_query_str( char * referer_str, char ** search_hostnames );
referer_url_t * parse_referer_str( char * referer_str, char ** internal_hostnames, char ** search_hostnames );
#ifdef __cplusplus
}
#endif
#endif
