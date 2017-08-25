#ifdef __cplusplus
#ifndef __UAP__
#include "uap.hpp"
#endif
extern "C" {
#endif
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
#ifndef __HTLOG_GEOIP__
#include "htlog_geoip.h"
#endif
#ifndef __HTTPACCESS_MYSQL__
#include "htlog_mysql.h"
#endif
#ifndef __HTTPACCESS_METRICS__
#ifndef __HTLOG_UAP__
#include "htlog_uap.h"
#endif
#ifndef __HTLOG_URLS__
#include "htlog_urls.h"
#endif
typedef struct httpaccess_metrics {
  int st; // earliest timestamp in log
  int et; // latest timestamp in log
  char *error;
  int real_did;
  int uid;
  int lines_failed;
  int lines_processed;
  int num_ihosts;
  char ** internal_hostnames;
  int num_shosts;
  char ** search_hostnames;
  hashtable_t * client_ips;             // ip_id
  hashtable_t * client_geo_locations;   // country_id
  hashtable_t * client_devices;         // with client_devices vers device_id 
  hashtable_t * client_oses;            // os_id
  hashtable_t * client_browsers;        // browser_id
  hashtable_t * page_paths;             // page_id, needs domains_id
  hashtable_t * referer_hostnames;      // referer_domain_id
  hashtable_t * referer_pathstrings;    // with referer_hostnames: referer_id, referer_domain_id
  hashtable_t * referer_paramstrings;   // with internref_hostnames, internref_pathstrings, internref_paramstrings: domain_id, page_id, url_param_id
  hashtable_t * internref_hostnames;    // domain_id
  hashtable_t * internref_pathstrings;  // page_id
  hashtable_t * internref_paramstrings; // url_param_id
  hashtable_t * search_queries;         // search_term_id
  hashtable_t * tvectors_inner;         // needs page_id
  hashtable_t * tvectors_inner_per_hour;// needs page_id
  hashtable_t * tvectors_incoming;      // needs referer_id
  hashtable_t * tvectors_inc_per_hour;  // needs referer_id
  hashtable_t * hits;                   // needs page_id
  hashtable_t * visits;                 // needs ip_id entities
  hashtable_t * pageviews;              // needs page_id, ip_id
} httpaccess_metrics;
#define __HTTPACCESS_METRICS__
httpaccess_metrics* h_metrics_init( int real_did, int uid,
  char ** user_hostnames, int nihosts, char ** search_hostnames, int nshosts);
httpaccess_metrics* h_metrics_reset_hashtables( httpaccess_metrics* h_metrics );
char *h_metrics_get_error( httpaccess_metrics *h_metrics );
void h_metrics_clear_error( httpaccess_metrics *h_metrics);
void h_metrics_free( httpaccess_metrics *h_metrics);
int h_metrics_process_line( httpaccess_metrics *h_metrics, char *l); 
int logs_scan( httpaccess_metrics *h_metrics, char *filename);
int stats_process_user_ips( httpaccess_metrics *h_metrics, char *user_ip );
int stats_process_geo_locations( httpaccess_metrics *h_metrics, char *user_ip );
int stats_process_ua( httpaccess_metrics *h_metrics, char *ua_str );
int stats_process_page_paths( httpaccess_metrics *h_metrics, char *page_path );
int stats_process_referer_str( httpaccess_metrics *h_metrics, char *referer_str );
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
#ifndef __SHARED_ARGS__
#include <unistd.h>
typedef struct func_args {
  char * filename;
  long start_line;
  long num_lines;
  int line_index;
  int tid;
//  semaphore_t * sem;
} func_args_t;
typedef struct shared_arg {
  int func_id;
  int curr_id;
  int num_funcs;
  httpaccess_metrics* h_metrics;
  func_args_t **  func_args;
} shared_arg_t;
void * longs_scan_parallel( void * arg );
int lines_count( char * filename );
func_args_t * init_func_args( int tid, char * filename, long start_line, long num_lines, int locked );
void free_func_args_t( func_args_t * arguments );
void free_shared_args( shared_arg_t * prog_args);
#define __SHARED_ARGS__
#endif
#include <stddef.h> //uint32_t
#include <stdint.h> //uint32_t
void print_logline_header( );
int h_metrics_parse_line( logline *ll, char *l);
void print_logline( logline * ll );
int stats_counter_incr( hashtable_t *table, char *key, char * which );
int stats_name_version_node_incr( hashtable_t *table, char * name, char * version, char * which );
uint32_t get_ip_by_dns( char * hostname , char* ip);
unsigned long get_numeric_ip( char* ip );
int process_logfile( char* filename  );
#endif
#ifdef __cplusplus
}
#endif
