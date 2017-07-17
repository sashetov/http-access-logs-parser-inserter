#ifndef __HTLOG_URLS__
#include "htlog_urls.h"
#endif
char * get_referer_url( char * referer_str, char ** internal_hostnames ){
  return "fake referer url";
}
char * get_search_query_str( char * referer_str, char ** search_hostnames ){
  return "fake search query str";
}
referer_url_t * parse_referer_str( char * referer_str, 
    char ** internal_hostnames, char ** search_hostnames ){
  //:https://www.atthematch.com/article/power-of-sport-to-promote
  referer_url_t * ref;
  return ref;
}
