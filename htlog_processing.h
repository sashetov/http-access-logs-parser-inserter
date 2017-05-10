#define LINE_MAX                1000
#define CONFIG_STREAM_MODE      0
#define CONFIG_TIME_DELTA       0
#define CONFIG_DEBUG            1
#ifndef __HT_NODE__
#include "ht_nodes.h"
#endif
#ifndef __HASHTAB_H__
#include "hashtab.h"
#endif
#ifndef __HASHTABLE_H__
#include "hashtable.h"
#endif
#ifndef __HTTPACCESS_METRICS__
typedef struct httpaccess_metrics {
  int st; // earliest timestamp in log
  int et; // latest timestamp in log
  char *error;
  int real_did;
  int uid;
  int lines_failed;
  int lines_processed;
  hashtable_t * per_hour_distinct_did_access_count__hits;
  hashtable_t * client_ips;
  hashtable_t * client_geo_location;
  hashtable_t * per_hour_distinct_did_cip_access_count__visits;
  hashtable_t * client_ua_str;
  hashtable_t * client_browser_vers;
  hashtable_t * client_oses_vers;
  hashtable_t * client_platform;
  hashtable_t * page_paths;
  hashtable_t * per_hour_distinct_did_pid_cip_access_count__pageviews;
  hashtable_t * tvectors_inner;
  hashtable_t * per_hour_tvectors_inner;
  hashtable_t * referer_urls;
  hashtable_t * search_qstr;
  hashtable_t * tvectors_incoming;
  hashtable_t * per_hour_tvectors_inc;
} httpaccess_metrics;
#define __HTTPACCESS_METRICS__
httpaccess_metrics * h_metrics_init( int real_did, int uid );
void h_metrics_reset_hashtables( httpaccess_metrics *h_metrics);
char *h_metrics_get_error( httpaccess_metrics *h_metrics );
void h_metrics_clear_error( httpaccess_metrics *h_metrics);
void h_metrics_free( httpaccess_metrics *h_metrics);
int h_metrics_process_line( httpaccess_metrics *h_metrics, char *l); 
int logs_scan( httpaccess_metrics *h_metrics, char *filename);
int print_all_ips( httpaccess_metrics *h_metrics );
int stats_process_user_ips( httpaccess_metrics *h_metrics, char *user_ip );
int print_all_ips( httpaccess_metrics *h_metrics );
#endif
#ifndef __LOG_LINE__
typedef struct logline {
  char *host;
  char *user_hostname;
  char *date;
  char *hour;
  char *timezone;
  char *req;
  char *ref;
  char *agent;
  time_t time;
  struct tm tm;
} logline;
#define __LOG_LINE__
#include <stddef.h> //uint32_t
#include <stdint.h> //uint32_t
void print_logline_header( );
int hg_metrics_parse_line( logline *ll, char *l);
void print_logline( logline * ll );
int stats_counter_incr( hashtable_t *table, char *key);
uint32_t get_ip_by_dns( char * hostname , char* ip);
int scan_file_to_loglines( char* filename  );
#endif
