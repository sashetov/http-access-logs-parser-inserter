#if !defined( __HTTPACCESS_METRICS__ ) || ! defined ( __LOG_LINE__ )
#include "htlog_processing.h"
httpaccess_metrics *h_metrics_init(void) {
  httpaccess_metrics *h_metrics;
  if ((h_metrics = malloc(sizeof(* h_metrics ))) == NULL) {
    return NULL;
  }
  h_metrics->error = NULL;
  h_metrics->st= h_metrics->et = time(NULL);
  h_metrics->lines_processed= 0;
  h_metrics->lines_failed   = 0;
  ht_init(&h_metrics->per_hour_distinct_did_access_count__hits, HT_ALLOC_SIZE_MAX);
  ht_init(&h_metrics->client_ips, HT_ALLOC_SIZE_MAX);
  ht_init(&h_metrics->client_geo_location, HT_ALLOC_SIZE_MAX);
  ht_init(&h_metrics->per_hour_distinct_did_cip_access_count__visits, HT_ALLOC_SIZE_MAX);
  ht_init(&h_metrics->client_ua_str, HT_ALLOC_SIZE_MAX);
  ht_init(&h_metrics->client_browser_vers, HT_ALLOC_SIZE_MAX);
  ht_init(&h_metrics->client_oses_vers, HT_ALLOC_SIZE_MAX);
  ht_init(&h_metrics->client_platform, HT_ALLOC_SIZE_MAX);
  ht_init(&h_metrics->page_paths, HT_ALLOC_SIZE_MAX);
  ht_init(&h_metrics->per_hour_distinct_did_pid_cip_access_count__pageviews, HT_ALLOC_SIZE_MAX);
  ht_init(&h_metrics->tvectors_inner, HT_ALLOC_SIZE_MAX);
  ht_init(&h_metrics->per_hour_tvectors_inner, HT_ALLOC_SIZE_MAX);
  ht_init(&h_metrics->referer_urls, HT_ALLOC_SIZE_MAX);
  ht_init(&h_metrics->search_qstr, HT_ALLOC_SIZE_MAX);
  ht_init(&h_metrics->tvectors_incoming, HT_ALLOC_SIZE_MAX);
  ht_init(&h_metrics->per_hour_tvectors_inc, HT_ALLOC_SIZE_MAX);
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
void h_metrics_reset_hashtables(httpaccess_metrics *h_metrics) {
  htab_empty(h_metrics->per_hour_distinct_did_access_count__hits);
  htab_empty(h_metrics->client_ips);
  htab_empty(h_metrics->client_geo_location);
  htab_empty(h_metrics->per_hour_distinct_did_cip_access_count__visits);
  htab_empty(h_metrics->client_ua_str);
  htab_empty(h_metrics->client_browser_vers);
  htab_empty(h_metrics->client_oses_vers);
  htab_empty(h_metrics->client_platform);
  htab_empty(h_metrics->page_paths);
  htab_empty(h_metrics->per_hour_distinct_did_pid_cip_access_count__pageviews);
  htab_empty(h_metrics->tvectors_inner);
  htab_empty(h_metrics->per_hour_tvectors_inner);
  htab_empty(h_metrics->referer_urls);
  htab_empty(h_metrics->search_qstr);
  htab_empty(h_metrics->tvectors_incoming);
  htab_empty(h_metrics->per_hour_tvectors_inc);
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
void h_metrics_free(httpaccess_metrics *h_metrics) {
  if (!h_metrics) {
    return;
  }
  h_metrics_reset_hashtables(h_metrics);
  h_metrics_clear_error(h_metrics);
  free(h_metrics);
}
int h_metrics_parse_line(logline *ll, char *l) {
  //domain.tld userhostname.dns.tld - - [03/Apr/2017:00:02:29 +0000] "GET /feed/ HTTP/1.0" 200 55490 "-" "-" 185.41.10.139
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
int stats_counter_incr( htab_t *ht, char *key ) {
  char *k;
  unsigned int idx;
  int r;
  long val;
  const PTR el;
  ht_kadd_val_to_k_nval( ht, key, (void*) 1);
  node *kn= node_init(key);
  node *n = (node *)htab_find(ht, kn);
  if( n != NULL ){
    n->nval++;
  }
  else {
    n = kn;
    free(k);
  }
  return n->nval;
}
int stats_process_user_ips( httpaccess_metrics *h_metrics, char *user_ip ){
  int res = stats_counter_incr( &h_metrics->client_ips, user_ip);
  if (res == 0) {
    return 1;
  }
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
  char origline[LINE_MAX];
  if (h_metrics_parse_line(&ll, l) == 0) {
    h_metrics->lines_processed++;
    print_logline( &ll );
    if (stats_process_user_ips( h_metrics, ll.user_hostname ) ) {
      goto oom;
    }
    return 0;
  }
  else {
    h_metrics->lines_failed++;
    if (CONFIG_DEBUG) {
      fprintf( stderr, "Invalid line: %s\n", origline);
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
  if ( (he = gethostbyname( hostname ) ) == NULL) {
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
int print_all_ips( httpaccess_metrics *h_metrics ){
  //int ips_len= ht_used(&h_metrics->user_ips);
  //int i;
  //printf("ips_len: %d\n",ips_len);
  /*if ((table = ht_get_array(&h_metrics->user_ips)) == NULL) {
    fprintf(stderr, "Out of memory in print_all_ips()\n");
    return 1;
  }*/
  //char* ip;
  //uint32_t ip_n=0;
  //char ip_real[50];
  //qsort(table, ips_len, sizeof(void*)*2, qsort_cmp_long_value);
  /*for (i = 0; i < ips_len*2 ; i+=2) {
    ip = (char *) table[i];      // key
    unsigned int idx;
    int r = ht_search(&h_metrics->user_ips,ip, &idx);
    long visits = (long) ht_value( &h_metrics->user_ips, idx );
    // inet_aton is in host byte order 
    // htonl to make it to network byte order for mysql
    ip_n = htonl(get_ip_by_dns(ip, ip_real));
    if( ip_n !=0 && ip_n != 1 ){ 
      printf("%lu %u %s %s\n", visits, ip_n, ip, ip_real);
    }
  }*/
  //free(table);
  return 0;
}
int scan_file_to_loglines( char* filename  ) {
  httpaccess_metrics *h_metrics;
  h_metrics = h_metrics_init();
  if ( logs_scan( h_metrics, filename ) ) {
    char *err= h_metrics_get_error( h_metrics );
    if( err ) {
      printf(stderr, "%s\n", err);
      exit(1);
    }
  }
  h_metrics_free(h_metrics);
}
#endif
