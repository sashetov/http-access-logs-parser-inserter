#ifndef __HTLOG_URLS__
#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
char * get_params_str( char * path );
int get_num_params( char * params_str );
typedef struct url_params {
  char * key;
  char * value;
} url_params_t;
url_params_t ** url_params_init( char * params_str, int num_params );
void url_params_free( url_params_t ** params, int len );
#define URL_EXTERNAL_NOSEARCH 0
#define URL_SEARCH            1
#define URL_INTERNAL          2
typedef struct referer_url {
  int   is_internal; // see defines above
  char * hostname;
  char * path_str;
  char * params_str;
  int n_params;
  url_params_t ** params;
} referer_url_t;
referer_url_t * referer_url_init( int internal, char * hostname, char * path_str, char * params_str );
void  free_referer_url(referer_url_t* ref);
referer_url_t * parse_referer_str( char * referer_str, int nih, char ** internal_hostnames, int nsh, char ** search_hostnames);
#ifdef __cplusplus
}
#endif
#endif
