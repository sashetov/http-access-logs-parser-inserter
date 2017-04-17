typedef struct httpaccess_metrics {
  int st; // earliest timestamp in log
  int et; // latest timestamp in log
  char *error;
  int real_did;
  int uid;
  int lines_failed;
  int lines_processed;
  htab_t per_hour_distinct_did_access_count__hits;
  htab_t client_ips;
  htab_t client_geo_location;
  htab_t per_hour_distinct_did_cip_access_count__visits;
  htab_t client_ua_str;
  htab_t client_browser_vers;
  htab_t client_oses_vers;
  htab_t client_platform;
  htab_t page_paths;
  htab_t per_hour_distinct_did_pid_cip_access_count__pageviews;
  htab_t tvectors_inner;
  htab_t per_hour_tvectors_inner;
  htab_t referer_urls;
  htab_t search_qstr;
  htab_t tvectors_incoming;
  htab_t per_hour_tvectors_inc;
} httpaccess_metrics;
httpaccess_metrics *h_metrics_init(void);
void h_metrics_set_error(httpaccess_metrics *h_metrics, char *fmt, ...);
void h_metrics_reset_hashtables(httpaccess_metrics *h_metrics);
void h_metrics_set_error(httpaccess_metrics *h_metrics, char *fmt, ...);
void h_metrics_clear_error(httpaccess_metrics *h_metrics);
void h_metrics_free(httpaccess_metrics *h_metrics);
int h_metrics_parse_line(struct logline *ll, char *l);
struct logline {
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
};
void print_logline_header( );
void print_logline( struct logline * ll );
int stats_counter_incr(struct htab_t *ht, char *key);
int stats_process_user_ips( httpaccess_metrics *h_metrics, char *user_ip );
int h_metrics_process_line(struct httpaccess_metrics *h_metrics, char *l); 
int stats_process_user_ips( httpaccess_metrics *h_metrics, char *user_ip );
int logs_scan(httpaccess_metrics *h_metrics, char *filename);
uint32_t get_ip_by_dns(char * hostname , char* ip);
int print_all_ips( httpaccess_metrics *h_metrics );

  res = stats_counter_incr( &h_metrics->client_ips, user_ip);
  if (res == 0) {
    return 1;
  }
  return 0;
}
int h_metrics_process_line(struct httpaccess_metrics *h_metrics, char *l) {
  struct logline ll;
  char origline[LINE_MAX];
  if (h_metrics_parse_line(&ll, l) == 0) {
    h_metrics->lines_processed++;
    print_logline( &ll );
    if (stats_process_user_ips( /*h_metrics, ll.user_hostname*/) ) {
      goto oom;
    }
    return 0;
  }
  else {
    h_metrics->lines_failed++;
    if (CONFIG_DEBUG) {
      fprintf(stderr, "Invalid line: %s\n", origline);
    }
    return 0;
  }
oom:
  h_metrics_set_error(ent, "Out of memory processing data");
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
      h_metrics_set_error(h_metrics, "Unable to open '%s': '%s'", filename, strerror(errno));
      return 1;
    }
  }
  //print_logline_header();
  while (fgets(buf, LINE_MAX, fp) != NULL) {
    if (h_metrics_process_line(ent, buf)) {
      fclose(fp);
      fprintf(stderr, "%s: %s\n", filename, h_metrics_get_error(ent));
      return 1;
    }
  }
  if (!use_stdin)
    fclose(fp);
  h_metrics->endt = time(NULL);
  return 0;
}
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

