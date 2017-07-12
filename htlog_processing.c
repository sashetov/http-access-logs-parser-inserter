#if !defined( __HTTPACCESS_METRICS__ ) || ! defined ( __LOG_LINE__ )
#include <stdarg.h>
#ifndef _TIME_H
#include <time.h>
#endif
#include "htlog_processing.h"

httpaccess_metrics* h_metrics_init( int real_did, int uid ) {
  httpaccess_metrics *h_metrics;
  if ((h_metrics = malloc(sizeof( httpaccess_metrics ))) == NULL) {
    return ;
  }
  h_metrics->real_did = real_did;
  h_metrics->uid = uid;
  h_metrics->error = NULL;
  h_metrics->st= h_metrics->et = time(NULL);
  h_metrics->lines_processed= 0;
  h_metrics->lines_failed   = 0;
  h_metrics->hits = ht_create(HT_ALLOC_SIZE_DEFAULT,0,NULL);
  h_metrics->client_ips = ht_create(HT_ALLOC_SIZE_DEFAULT,0,NULL);
  h_metrics->client_geo_locations = ht_create(HT_ALLOC_SIZE_DEFAULT,0,NULL);
  h_metrics->visits = ht_create(HT_ALLOC_SIZE_DEFAULT,0,NULL);
  h_metrics->client_ua_str = ht_create(HT_ALLOC_SIZE_DEFAULT,0,NULL);
  h_metrics->client_browser_vers = ht_create(HT_ALLOC_SIZE_DEFAULT,0,NULL);
  h_metrics->client_oses_vers = ht_create(HT_ALLOC_SIZE_DEFAULT,0,NULL);
  h_metrics->client_platform = ht_create(HT_ALLOC_SIZE_DEFAULT,0,NULL);
  h_metrics->page_paths = ht_create(HT_ALLOC_SIZE_DEFAULT,0,NULL);
  h_metrics->pageviews= ht_create(HT_ALLOC_SIZE_DEFAULT,0,NULL);
  h_metrics->tvectors_inner = ht_create(HT_ALLOC_SIZE_DEFAULT,0,NULL);
  h_metrics->tvectors_inner_per_hour = ht_create(HT_ALLOC_SIZE_DEFAULT,0,NULL);
  h_metrics->referer_urls = ht_create(HT_ALLOC_SIZE_DEFAULT,0,NULL);
  h_metrics->search_qstr = ht_create(HT_ALLOC_SIZE_DEFAULT,0,NULL);
  h_metrics->tvectors_incoming = ht_create(HT_ALLOC_SIZE_DEFAULT,0,NULL);
  h_metrics->tvectors_inc_per_hour = ht_create(HT_ALLOC_SIZE_DEFAULT,0,NULL);
  return h_metrics;
}
httpaccess_metrics* h_metrics_reset_hashtables( httpaccess_metrics* h_metrics ) {
  free( h_metrics->hits);
  free( h_metrics->client_ips);
  free( h_metrics->client_geo_locations);
  free( h_metrics->visits);
  free( h_metrics->client_ua_str);
  free( h_metrics->client_browser_vers);
  free( h_metrics->client_oses_vers);
  free( h_metrics->client_platform);
  free( h_metrics->page_paths);
  free( h_metrics->pageviews);
  free( h_metrics->tvectors_inner);
  free( h_metrics->tvectors_inner_per_hour);
  free( h_metrics->referer_urls);
  free( h_metrics->search_qstr);
  free( h_metrics->tvectors_incoming);
  free( h_metrics->tvectors_inc_per_hour);
  return h_metrics;
}
void h_metrics_set_error(httpaccess_metrics *h_metrics, char *fmt, ...) {
  va_list ap;
  char buf[ERROR_MAX];
  va_start(ap, fmt);
  vsnprintf(buf, ERROR_MAX, fmt, ap);
  buf[ERROR_MAX-1] = '\0';
  free(h_metrics->error);
  h_metrics->error = strdup(buf);
  va_end(ap);
}
char *h_metrics_get_error( httpaccess_metrics *h_metrics) {
  if (!h_metrics->error) {
    return "No error";
  }
  return h_metrics->error;
}
void h_metrics_clear_error(httpaccess_metrics *h_metrics) {
  free(h_metrics->error);
  h_metrics->error = NULL;
}
void h_metrics_free( httpaccess_metrics *h_metrics ) {
  if (!h_metrics) {
    return;
  }
  h_metrics_reset_hashtables( h_metrics );
  h_metrics_clear_error( h_metrics );
  free( h_metrics );
}
int h_metrics_parse_line(logline *ll, char *l) {
  char *user_hostname, *date, *hour, *timezone, *host, *agent, *req, *ref, *p;
  char *agent_start = NULL, *req_end = NULL, *ref_end = NULL;
  int agent_without_parens = 0;
  //HOST
  host = l;
  //USER_IP
  if ((user_hostname= strchr(l, ' ')) == NULL){
    return 1;
  }
  user_hostname++;
  //DATE
  if ((date = strchr(l, '[')) == NULL){
    return 1;
  }
  date++;
  //AGENT
  if ((agent = strchr(l, '(')) == NULL) {
    char *aux = l, *last = NULL;
    int count = 0;
    while (*aux) {
      if (*aux == '"') {
        count++;
        last = aux;
      }
      aux++;
    }
    if (count == 6) {
      agent_without_parens = 1;
      agent_start = last-1;
      while(*agent_start != '"')
        agent_start--;
    } else {
      agent = "";
    }
  }
  else {
    p = agent;
    while (p >= l) {
      if (*p == '"') {
        agent_start = p;
        break;
      }
      p--;
    }
  }
  //REQ
  if ((req = strstr(l, "\"GET")) != NULL || (req = strstr(l, "\"POST")) != NULL || (req = strstr(l, "\"HEAD")) != NULL || (req = strstr(l, "\"get")) != NULL || (req = strstr(l, "\"post")) != NULL || (req = strstr(l, "\"head")) != NULL) {
    req++;
  }
  else {
    req = "";
  }
  //REF
  if ((ref = strstr(l, "\"http")) != NULL ||
      (ref = strstr(l, "\"HTTP")) != NULL) {
    ref++;
  }
  else {
    ref = "";
  }
  //HOST
  if ((p = strchr(host, ' ')) == NULL) {
    return 1;
  }
  *p = '\0'; // cut string via zero chars
  //USER_IP
  if ((p = strchr(user_hostname, ' ')) == NULL) {
    return 1;
  }
  *p = '\0';
  //DATE
  if ((p = strchr(date, ']')) == NULL) {
    return 1;
  }
  *p = '\0';
  //TIME
  if (ll->time == (time_t)-1) {
    return 1;
  }
  //DATE
  if ((p = strchr(date, ':')) == NULL) {
    return 1;
  }
  //HOUR
  hour = p+1;
  *p = '\0';
  if ((p = strchr(hour, ' ')) == NULL) {
    return 1;
  }
  //TIMEZONE
  timezone = p+1;
  *p = '\0';
  //REQ
  if ((p = strchr(req, '"')) == NULL) {
    req = "";
  }
  else {
    req_end = p;
    *p = '\0';
    if ((p = strchr(req, ' ')) != NULL) {
      req = p+1;
      if ((p = strchr(req, ' ')) != NULL) {
        *p = '\0';
      }
    }
  }
  //REF
  if ((p = strchr(ref, '"')) == NULL) {
    ref = "";
  }
  else {
    ref_end = p;
    *p = '\0';
  }
  //AGENT,REF,REQ
  if (agent_without_parens) {
    char *aux = strchr(agent_start+1, '"');
    if (!aux) {
      agent = "";
    }
    else {
      *aux = '\0';
      agent = agent_start+1;
    }
  }
  else if ((p = strchr(agent, ')')) == NULL) {
    agent = "";
  }
  else {
    char *aux;
    aux = strchr(p, '"');
    if (aux) {
      *aux = '\0';
    }
    else {
      *(p+1) = '\0';
    }
    if (agent_start) {
      if ((!req_end || (req_end != agent_start)) &&
          (!ref_end || (ref_end != agent_start))) {
        agent = agent_start+1;
      }
    }
  }
  ll->host = host;
  ll->user_hostname = user_hostname;
  ll->date = date;
  ll->hour = hour;
  ll->timezone = timezone;
  ll->agent = agent;
  ll->req = req;
  ll->ref = ref;
  return 0;
}
void print_logline_header( ) {
  printf ("host|user_hostname|date|hour|timezone|req|ref|agent\n");
}
void print_logline( logline * ll ) {
  printf ("%s|%s|%s|%s|%s|%s|%s|%s\n", 
      ll->host, ll->user_hostname, ll->date, ll->hour, 
      ll->timezone, ll->req, ll->ref, ll->agent);
}
int stats_counter_incr( hashtable_t *table, char* key ) {
  int val=1;
  if (! ht_exists(table, key, strlen(key) + 1 ) ) {
    node *n = node_init( key, 1 );
    ht_set(table, key, strlen(key)+1, n, sizeof(node *) );
    free(n);
  }
  else {
    node *n = (node *) ht_get_copy( table, key, strlen( key )+1, sizeof(node *) );
    n->nval++;
    val = n->nval;
    ht_set(table, key, strlen( key ) + 1, n, sizeof(node *));
    free(n);
  }
  return val;
}
int stats_process_user_ips( httpaccess_metrics *h_metrics, char *user_ip ){
  int res = stats_counter_incr( h_metrics->client_ips, user_ip );
  if (res == 0) {
    return 1;
  }
  return 0;
}
int stats_process_geo_locations( httpaccess_metrics *h_metrics, char *user_ip ){
  char * country_name = (char *) get_geoip_country(0, user_ip);
  int res = stats_counter_incr( h_metrics->client_geo_locations, country_name);
  if (res == 0) {
    return 1;
  }
  return 0;
}
int stats_process_ua( httpaccess_metrics *h_metrics, char *ua_str ){
  printf( "%s \n", ua_str );
  int res = stats_counter_incr( h_metrics->client_ua_str, ua_str );
  if (res == 0) {
    return 1;
  }
  return 0;
}
int stats_process_page_paths( httpaccess_metrics *h_metrics, char *page_path ){
  return 0;
}
int stats_process_referer_and_sqs( httpaccess_metrics *h_metrics, char *ref_str ){
  return 0;
}
int stats_process_hits( httpaccess_metrics *h_metrics ){
  return 0;
}
int stats_process_visits( httpaccess_metrics *h_metrics ){
  return 0;
}
int stats_process_pageviews( httpaccess_metrics *h_metrics ){
  return 0;
}
int stats_process_tvectors( httpaccess_metrics *h_metrics ){
  return 0;
}
#include <stdio.h>
#ifndef __uint32_t_defined
#include <stddef.h>
#define __uint32_t_defined
#endif
#include <string.h>
int h_metrics_process_line(httpaccess_metrics *h_metrics, char *l) {
  logline ll;
  if (h_metrics_parse_line(&ll, l) == 0) {
    //print_logline( &ll );
    if ( stats_process_user_ips( h_metrics, ll.user_hostname ) ) {
      goto oom;
    }
    if ( stats_process_geo_locations( h_metrics, ll.user_hostname ) ) {
      goto oom;
    }
    if ( stats_process_ua( h_metrics, ll.agent ) ) {
      goto oom;
    }
    if ( stats_process_page_paths( h_metrics, ll.req ) ) {
      goto oom;
    }
    if ( stats_process_referer_and_sqs( h_metrics, ll.ref ) ) {
      goto oom;
    }
    if ( stats_process_hits( h_metrics ) ) {
      goto oom;
    }
    if ( stats_process_visits( h_metrics ) ) {
      goto oom;
    }
    if ( stats_process_pageviews( h_metrics ) ) {
      goto oom;
    }
    if ( stats_process_tvectors( h_metrics ) ) {
      goto oom;
    }
    h_metrics->lines_processed++;
    return 0;
  }
  else {
    h_metrics->lines_failed++;
    if (CONFIG_DEBUG) {
      fprintf( stderr, "Invalid line: %s\n", l);
    }
    return 0;
  }
oom:
  h_metrics_set_error(h_metrics, "Out of memory processing data");
  return 1;
}
int logs_scan(httpaccess_metrics *h_metrics, char *filename) {
  FILE *fp;
  char buf[LINE_MAX];
  int use_stdin = 0;
  if (filename[0] == '-' && filename[1] == '\0') {
    if (CONFIG_STREAM_MODE)
      return 0;
    fp = stdin;
    use_stdin = 1;
  }
  else {
    if ((fp = fopen(filename, "r")) == NULL) {
      int errno;
      h_metrics_set_error(h_metrics, "Unable to open '%s': '%s'", filename, strerror(errno));
      return 1;
    }
  }
  //print_logline_header();
  while (fgets(buf, LINE_MAX, fp) != NULL) {
    if (h_metrics_process_line(h_metrics, buf)) {
      fclose(fp);
      fprintf(stderr, "%s: %s\n", filename, h_metrics_get_error(h_metrics));
      return 1;
    }
  }
  if (!use_stdin) {
    fclose(fp);
  }
  h_metrics->et = time(NULL);
  return 0;
}
#include <arpa/inet.h>
#include <netdb.h>
uint32_t get_ip_by_dns(char * hostname , char* ip) {
  struct hostent *he;
  struct in_addr **addr_list;
  int i;
  uint32_t ip_n = 0;
  if ( hostname && (he = gethostbyname( hostname ) ) == NULL) {
    fprintf(stderr, "gethostbyname failed for %s\n", hostname);
    return 1;
  }
  addr_list = (struct in_addr **) he->h_addr_list;
  for(i = 0; addr_list[i] != NULL; i++) {
    strcpy(ip , inet_ntoa(*addr_list[i]) );
    ip_n = addr_list[i]->s_addr;
    return ip_n;
  }
  return 1;
}
unsigned long get_numeric_ip(char* addr) {
  unsigned long c, octet, t;
  unsigned long ipnum;
  int i = 3;
  octet = ipnum = 0;
  while ((c = *addr++)) {
    if (c == '.') {
      if (octet > 255) {
        return 0;
      }
      ipnum <<= 8;
      ipnum += octet;
      i--;
      octet = 0;
    } else {
      t = octet;
      octet <<= 3;
      octet += t;
      octet += t;
      c -= '0';
      if (c > 9) {
        return 0;
      }
      octet += c;
    }
  }
  if ((octet > 255) || (i != 0)) {
    return 0;
  }
  ipnum <<= 8;
  return ipnum + octet;
}
int process_logfile( char* filename ) {
  httpaccess_metrics *h_metrics = h_metrics_init( 0, 0 );
  char * hostname = "atthematch.com";
  mysql_domain_resultset_t * drs = get_real_did_uid( hostname );
  h_metrics->real_did = drs->did;
  h_metrics->uid      = drs->uid;
  if ( logs_scan( h_metrics, filename ) ) {
    char *err= h_metrics_get_error( h_metrics );
    if( err ) {
      fprintf(stderr, "%s\n", err);
      exit(1);
    }
  }
  insert_h_metrics( h_metrics );
  h_metrics_free( h_metrics );
}
#endif
