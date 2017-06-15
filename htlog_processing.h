#define LINE_MAX                1000
#define CONFIG_STREAM_MODE      0
#define CONFIG_TIME_DELTA       0
#define CONFIG_DEBUG            1
#ifndef __HT_NODE__
#include "ht_nodes.h"
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
  hashtable_t * hits;               //per hour count for all urls under this did
  hashtable_t * client_ips;
  hashtable_t * client_geo_location;
  hashtable_t * visits;             //per hour distinct did,ip count
  hashtable_t * client_ua_str;
  hashtable_t * client_browser_vers;
  hashtable_t * client_oses_vers;
  hashtable_t * client_platform;
  hashtable_t * page_paths;
  hashtable_t * pageviews;          //per hour distinct did,ip,page_id count
  hashtable_t * tvectors_inner;
  hashtable_t * tvectors_inner_per_hour;
  hashtable_t * referer_urls;
  hashtable_t * search_qstr;
  hashtable_t * tvectors_incoming;
  hashtable_t * tvectors_inc_per_hour;
} httpaccess_metrics;
#define __HTTPACCESS_METRICS__
httpaccess_metrics* h_metrics_init( int real_did, int uid );
httpaccess_metrics* h_metrics_reset_hashtables( httpaccess_metrics* h_metrics );
char *h_metrics_get_error( httpaccess_metrics *h_metrics );
void h_metrics_clear_error( httpaccess_metrics *h_metrics);
void h_metrics_free( httpaccess_metrics *h_metrics);
int h_metrics_process_line( httpaccess_metrics *h_metrics, char *l); 
int logs_scan( httpaccess_metrics *h_metrics, char *filename);
int stats_process_user_ips( httpaccess_metrics *h_metrics, char *user_ip );
int stats_process_geo_location( httpaccess_metrics *h_metrics, char *user_ip );
int stats_process_ua( httpaccess_metrics *h_metrics, char *ua_str );
int stats_process_page_paths( httpaccess_metrics *h_metrics, char *page_path );
int stats_process_referer_and_sqs( httpaccess_metrics *h_metrics, char *ref_str );
int stats_process_hits( httpaccess_metrics *h_metrics );
int stats_process_visits( httpaccess_metrics *h_metrics );
int stats_process_pageviews( httpaccess_metrics *h_metrics );
int stats_process_tvectors( httpaccess_metrics *h_metrics );
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
int h_metrics_parse_line( logline *ll, char *l);
void print_logline( logline * ll );
int stats_counter_incr( hashtable_t *table, char *key);
uint32_t get_ip_by_dns( char * hostname , char* ip);
unsigned long get_numeric_ip( char* ip );
int process_logfile( char* filename  );
#endif
