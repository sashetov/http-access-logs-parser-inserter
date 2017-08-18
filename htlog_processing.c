#if !defined( __HTTPACCESS_METRICS__ ) || ! defined ( __LOG_LINE__ )
#include <stdarg.h>
#ifndef _TIME_H
#include <time.h>
#endif
#include "htlog_processing.h"
httpaccess_metrics* h_metrics_init(
    int real_did, int uid,
    char ** user_hostnames, int nihosts,
    char ** search_hostnames, int nshosts ) {
  int i =0;
  httpaccess_metrics *h_metrics;
  if ((h_metrics = (httpaccess_metrics * )malloc(sizeof( httpaccess_metrics ))) == NULL) {
    return h_metrics;
  }
  h_metrics->num_ihosts = nihosts;
  for(i = 0; i< h_metrics->num_ihosts; i++) {
    h_metrics->internal_hostnames = (char **) malloc(sizeof(user_hostnames));
    h_metrics->internal_hostnames[i] = (char *) malloc(strlen(user_hostnames[i])+1);
    strcpy(h_metrics->internal_hostnames[i], user_hostnames[i]);
  }
  h_metrics->num_shosts= nshosts;
  for(i = 0; i< h_metrics->num_shosts; i++) {
    h_metrics->search_hostnames = (char **) malloc(sizeof(search_hostnames));
    h_metrics->search_hostnames[i] = (char *) malloc(strlen(search_hostnames[i])+1);
    strcpy(h_metrics->search_hostnames[i], search_hostnames[i]);
  }
  h_metrics->real_did = real_did;
  h_metrics->uid = uid;
  h_metrics->error = NULL;
  h_metrics->st= h_metrics->et = time(NULL);
  h_metrics->lines_processed= 0;
  h_metrics->lines_failed   = 0;
  h_metrics->hits = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->client_ips = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->client_geo_locations = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->visits = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->client_ua_str = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->client_devices = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->client_oses = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->client_browsers = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->client_devices_vers = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->client_oses_vers = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->client_browsers_vers = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->page_paths = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->referer_hostnames = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->referer_pathstrings = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->referer_paramstrings = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->internref_hostnames = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->internref_pathstrings = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->internref_paramstrings = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->search_queries = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->pageviews = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->tvectors_inner = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->tvectors_inner_per_hour = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->tvectors_incoming = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  h_metrics->tvectors_inc_per_hour = ht_create(HT_ALLOC_SIZE_DEFAULT,HT_ALLOC_SIZE_MAX,NULL);
  return h_metrics;
}
httpaccess_metrics* h_metrics_reset_hashtables( httpaccess_metrics* h_metrics ) {
  int i;
  for(i = 0; i< h_metrics->num_ihosts; i++) {
    free(h_metrics->internal_hostnames[i]);
  }
  free( h_metrics->internal_hostnames );
  for(i = 0; i< h_metrics->num_shosts; i++) {
    free( h_metrics->search_hostnames[i] );
  }
  free( h_metrics->search_hostnames );
  ht_destroy( h_metrics->hits);
  ht_destroy( h_metrics->client_ips);
  ht_destroy( h_metrics->client_geo_locations);
  ht_destroy( h_metrics->visits);
  ht_destroy( h_metrics->client_ua_str);
  ht_destroy( h_metrics->client_devices);
  ht_destroy( h_metrics->client_oses);
  ht_destroy( h_metrics->client_browsers);
  ht_destroy( h_metrics->client_devices_vers);
  ht_destroy( h_metrics->client_oses_vers);
  ht_destroy( h_metrics->client_browsers_vers);
  ht_destroy( h_metrics->page_paths);
  ht_destroy( h_metrics->referer_hostnames);
  ht_destroy( h_metrics->referer_pathstrings);
  ht_destroy( h_metrics->referer_paramstrings);
  ht_destroy( h_metrics->internref_hostnames);
  ht_destroy( h_metrics->internref_pathstrings);
  ht_destroy( h_metrics->internref_paramstrings);
  ht_destroy( h_metrics->search_queries);
  ht_destroy( h_metrics->pageviews);
  ht_destroy( h_metrics->tvectors_inner);
  ht_destroy( h_metrics->tvectors_inner_per_hour);
  ht_destroy( h_metrics->tvectors_incoming);
  ht_destroy( h_metrics->tvectors_inc_per_hour);
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
  if ((req = strstr(l, "\"GET")) != NULL ||
      (req = strstr(l, "\"POST")) != NULL || 
      (req = strstr(l, "\"HEAD")) != NULL || 
      (req = strstr(l, "\"get")) != NULL || 
      (req = strstr(l, "\"post")) != NULL || 
      (req = strstr(l, "\"head")) != NULL) {
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
  //if (ll->time == (time_t)-1) {
  //  return 1;
  //}
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
  //printf("key: '%s'\n",key);
  if(strlen(key) == 0){
    return val;
  }
  if (! ht_exists(table, key, strlen(key) + 1 ) ) {
    node *n = node_init( key, 1 );
    ht_set(table, key, strlen(key)+1, n, sizeof(node));
    //printf("key: '%s'\n",key);
    // don't free this node here as we're using ht_set not ht_set_copy
    //free_node(n);
  }
  else {
    node *n = (node *) ht_get_copy( table, key, strlen( key )+1, (size_t*) sizeof(node));
    n->nval++;
    val = n->nval;
    ht_set(table, key, strlen( key ) + 1, n, sizeof(node));
    // don't free this node as we're using ht_set not ht_set_copy
    //free_node(n);
  }
  return val;
}
int stats_process_user_ips( httpaccess_metrics *h_metrics, char *user_ip ){
  //printf("processing user_ips \n");
  int res = stats_counter_incr( h_metrics->client_ips, user_ip );
  if (res == 0) {
    return 1;
  }
  return 0;
}
int stats_process_geo_locations( httpaccess_metrics *h_metrics, char *user_ip ){
  //printf("processing geoips \n");
  char * country_name = (char *) get_geoip_country(0, user_ip);
  int res = stats_counter_incr( h_metrics->client_geo_locations, country_name);
  if (res == 0) {
    return 1;
  }
  return 0;
}
int stats_process_ua( httpaccess_metrics *h_metrics, char *ua_str ){
  //printf("processing ua \n");
  ua_t * ua_parsed = (ua_t *) parse_to_c_ua( ua_str );
  char * device_vers_template= "%s %s";
  char * os_version = get_version_string(
      ua_parsed->os->major, ua_parsed->os->minor, ua_parsed->os->patch, "");
  char * browser_version = get_version_string(
      ua_parsed->browser->major,
      ua_parsed->browser->minor,
      ua_parsed->browser->patch,
      ua_parsed->browser->patch_minor );
  char * device_vers = (char *) malloc( snprintf( NULL, 0,
        device_vers_template,
        ua_parsed->device->brand, ua_parsed->device->model) + sizeof(char));
  sprintf( device_vers , device_vers_template,
      ua_parsed->device->brand, ua_parsed->device->model);
  if (stats_counter_incr(
        h_metrics->client_ua_str, ua_str ) == 0) {
    free_c_ua(ua_parsed);
    free( device_vers );
    return 1;
  } //else printf("stats_counter_incr(client_ua_str,%s)\n", ua_str );
  if (stats_counter_incr(
        h_metrics->client_devices, ua_parsed->device->family ) == 0) {
    free_c_ua(ua_parsed);
    free( device_vers );
    return 2;
  } //else printf("stats_counter_incr(client_devices,%s)\n", ua_parsed->device->family);
  if (stats_counter_incr(
        h_metrics->client_oses , ua_parsed->os->family ) == 0) {
    free_c_ua(ua_parsed);
    free( device_vers );
    return 3;
  } //else printf("stats_counter_incr(client_oses,%s)\n", ua_parsed->os->family);
  if (stats_counter_incr(
        h_metrics->client_browsers, ua_parsed->browser->family ) == 0) {
    free_c_ua(ua_parsed);
    free( device_vers );
    return 4;
  } //else printf("stats_counter_incr(client_browsers,%s)\n", ua_parsed->browser->family);
  if (stats_counter_incr(
        h_metrics->client_devices, ua_parsed->device->family ) == 0) {
    free_c_ua(ua_parsed);
    free( device_vers );
    return 5;
  } //else printf("stats_counter_incr(client_devices,%s)\n", ua_parsed->device->family);
  if (stats_counter_incr(
        h_metrics->client_devices_vers, device_vers) == 0) {
    free_c_ua(ua_parsed);
    free( device_vers );
    return 6;
  } //else printf("stats_counter_incr(devices_vers,%s)\n", device_vers);
  if (stats_counter_incr(
        h_metrics->client_browsers_vers, browser_version ) == 0) {
    free_c_ua(ua_parsed);
    free( device_vers );
    return 7;
  } //else printf("stats_counter_incr(browsers_vers,%s)\n", browser_version);
  if (stats_counter_incr(
        h_metrics->client_oses_vers, os_version ) == 0) {
    free_c_ua(ua_parsed);
    free( device_vers );
    return 8;
  } //else printf("stats_counter_incr(oses_vers,%s)\n", os_version);
  free_c_ua(ua_parsed);
  free( device_vers );
  //free( browser_version ); // valgrind claims r_x mapped sector for these...
  //free( os_version );
  return 0;
}
int stats_process_page_paths( httpaccess_metrics *h_metrics, char *page_path ){
  //printf("processing page_paths \n");
  if (stats_counter_incr(
        h_metrics->page_paths, page_path) == 0) {
    return 1;
  }// else printf("stats_counter_incr(page_paths,%s)\n", page_path);
  return 0;
}
int stats_process_referer_str( httpaccess_metrics *h_metrics, char *referer_str ){
  char * qstr;
  int length =0;
  referer_url_t * parsed_url;
  parsed_url = parse_referer_str( referer_str, 
      h_metrics->num_ihosts, h_metrics->internal_hostnames,
      h_metrics->num_shosts, h_metrics->search_hostnames );
  //printf("processing parsed_urls \n");
  if( parsed_url && parsed_url->is_internal == URL_INTERNAL ) {
    if (stats_counter_incr( h_metrics->internref_hostnames, parsed_url->hostname) == 0 ) {
      free_referer_url(parsed_url);
      return 1;
    }
    if (stats_counter_incr( h_metrics->internref_pathstrings, parsed_url->path_str) == 0 ) {
      free_referer_url(parsed_url);
      return 2;
    }
    if (stats_counter_incr( h_metrics->internref_paramstrings, parsed_url->params_str) == 0 ) {
      free_referer_url(parsed_url);
      return 3;
    }
  }
  else if(  parsed_url && parsed_url->is_internal == URL_EXTERNAL_NOSEARCH ){
    if (stats_counter_incr(
          h_metrics->referer_hostnames, parsed_url->hostname) == 0 ) {
      free_referer_url(parsed_url);
      return 5;
    }
    if (stats_counter_incr(
          h_metrics->referer_pathstrings, parsed_url->path_str) == 0 ) {
      free_referer_url(parsed_url);
      return 5;
    }
    if (stats_counter_incr( 
          h_metrics->referer_paramstrings, parsed_url->params_str) == 0 ) {
      free_referer_url(parsed_url);
      return 6;
    }
  }
  else if(  parsed_url && parsed_url->is_internal == URL_SEARCH ) {
    length = snprintf( NULL, 0, "%s:%s", parsed_url->hostname, parsed_url->params_str ) + 1;
    qstr = (char *) malloc( length );
    snprintf( qstr, length, "%s:%s", parsed_url->hostname, parsed_url->params_str );
    if ( stats_counter_incr( h_metrics->search_queries, qstr) == 0 ) {
      free_referer_url(parsed_url);
      free(qstr);
      return 7;
    }
    free(qstr);
  }
  else if(parsed_url){
    free_referer_url(parsed_url);
  }
  return 0;
}
int stats_process_hits( httpaccess_metrics *h_metrics ){
  //printf("processing hits \n");
  int length = snprintf( NULL, 0, "%d", h_metrics->real_did ) + 1;
  char* real_did_str = (char *)malloc( length );
  snprintf( real_did_str, length, "%d", h_metrics->real_did );
  if (stats_counter_incr(
        h_metrics->hits, real_did_str) == 0 ) {
    return 1;
  }
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
    if ( stats_process_referer_str( h_metrics, ll.ref ) ) {
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
      h_metrics_set_error(
          h_metrics, "Unable to open '%s': '%s'", filename,
          strerror(errno));
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
  char * hostname = "atthematch.com";
  char ** hostnames = (char **) malloc( sizeof(char *) );
  //hostnames[0]=(char*)malloc(strlen( hostname )+1);
  //strcpy(hostnames[0],hostname);
  hostnames[0] = strdup(hostname);
  char **search_hostnames = (char **)malloc(sizeof(char*));
  search_hostnames[0] = (char *)"google.com";
  httpaccess_metrics *h_metrics = h_metrics_init(
      0, 0, hostnames, 1, search_hostnames, 1 );
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
  free(hostnames[0]);
  free(hostnames);
  free(search_hostnames);
}
#endif
