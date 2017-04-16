#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <locale.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <search.h>

#define ERROR_MAX          10240
#define LINE_MAX           1000000
#define CONFIG_DEBUG       1
#define CONFIG_STREAM_MODE 0
#define CONFIG_TIME_DELTA  0

struct entities {
  int startt;
  int endt;
  int processed;
  int invalid;
  int blacklisted;
  int hour[24];
  int weekday[7];
  int weekdayhour[7][24];
  int monthday[12][31];
  struct hashtable pages;
  struct hashtable user_ips;
  struct hashtable pageviews;
  struct hashtable referers;
  struct hashtable date;
  struct hashtable month;
  struct hashtable agents;
  struct hashtable trails;
  struct hashtable os;
  struct hashtable browsers;
  char *error;
  */
};
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
void entities_reset_combined_maps(struct entities * ent) {
  int i, j;
  for (i = 0; i < 24; i++) {
    ent->hour[i] = 0;
    for (j = 0; j < 7; j++){
      ent->weekdayhour[j][i] = 0;
    }
  }
  for (i = 0; i < 7; i++) {
    ent->weekday[i] = 0;
  }
  for (i = 0; i < 31; i++) {
    for (j = 0; j < 12; j++) {
      ent->monthday[j][i] = 0;
    }
  }
}
void entities_ht_init(struct hashtable *ht, int size) {
  (void) hcreate(size);
  ht_init(ht);
  ht_set_hash(ht, ht_hash_string);
  ht_set_key_destructor(ht, ht_destructor_free);
  ht_set_val_destructor(ht, ht_no_destructor);
  ht_set_key_compare(ht, ht_compare_string);
}
struct entities *entities_new(void) {
  struct entities *ent;
  if ((ent = malloc(sizeof(*ent))) == NULL) {
    return NULL;
  }
  ent->startt = ent->endt = time(NULL);
  ent->processed = 0;
  ent->invalid = 0;
  ent->blacklisted = 0;
  entities_reset_combined_maps(ent);
  ent->error = NULL;

  entities_ht_init(&ent->pages);
  entities_ht_init(&ent->user_ips);
  entities_ht_init(&ent->pageviews);
  entities_ht_init(&ent->referers);
  entities_ht_init(&ent->date);
  entities_ht_init(&ent->month);
  entities_ht_init(&ent->agents);
  entities_ht_init(&ent->trails);
  entities_ht_init(&ent->os);
  entities_ht_init(&ent->browsers);

  return ent;
}
void entities_set_error(struct entities *ent, char *fmt, ...) {
  va_list ap;
  char buf[ERROR_MAX];
  va_start(ap, fmt);
  vsnprintf(buf, ERROR_MAX, fmt, ap);
  buf[ERROR_MAX-1] = '\0';
  free(ent->error);
  ent->error = strdup(buf);
  va_end(ap);
}
void entities_reset_hashtables(struct entities *ent) {
  ht_destroy(&ent->pages);
  ht_destroy(&ent->pageviews);
  ht_destroy(&ent->referers);
  ht_destroy(&ent->date);
  ht_destroy(&ent->month);
  ht_destroy(&ent->agents);
  ht_destroy(&ent->trails);
  ht_destroy(&ent->os);
  ht_destroy(&ent->browsers);
}
char *entities_get_error(struct entities *ent) {
  if (!ent->error) {
    return "No error";
  }
  return ent->error;
}
void entities_clear_error(struct entities *ent) {
  free(ent->error);
  ent->error = NULL;
}
void entities_free(struct entities *ent) {
  if (!ent) {
    return;
  }
  entities_reset_hashtables(ent);
  entities_clear_error(ent);
  free(ent);
}
int entities_match_len(const char *pattern, int patternLen, const char *string, int stringLen, int nocase) {
  while(patternLen) {
    switch(pattern[0]) {
      case '*':
        while (pattern[1] == '*') {
          pattern++;
          patternLen--;
        }
        if (patternLen == 1) {
          return 1; /* match */
        }
        while(stringLen) {
          if (entities_match_len( pattern+1, patternLen-1,
                string, stringLen, nocase)){
            return 1; /* match */
          }
          string++;
          stringLen--;
        }
        return 0; /* no match */
        break;
      case '?':
        if (stringLen == 0)
          return 0; /* no match */
        string++;
        stringLen--;
        break;
      case '[':
        {
          int not, match;
          pattern++;
          patternLen--;
          not = pattern[0] == '^';
          if (not) {
            pattern++;
            patternLen--;
          }
          match = 0;
          while(1) {
            if (pattern[0] == '\\') {
              pattern++;
              patternLen--;
              if (pattern[0] == string[0])
                match = 1;
            } else if (pattern[0] == ']') {
              break;
            } else if (patternLen == 0) {
              pattern--;
              patternLen++;
              break;
            } else if (pattern[1] == '-' && patternLen >= 3) {
              int start = pattern[0];
              int end = pattern[2];
              int c = string[0];
              if (start > end) {
                int t = start;
                start = end;
                end = t;
              }
              if (nocase) {
                start = tolower(start);
                end = tolower(end);
                c = tolower(c);
              }
              pattern += 2;
              patternLen -= 2;
              if (c >= start && c <= end)
                match = 1;
            } else {
              if (!nocase) {
                if (pattern[0] == string[0])
                  match = 1;
              } else {
                if (tolower((int)pattern[0]) == tolower((int)string[0]))
                  match = 1;
              }
            }
            pattern++;
            patternLen--;
          }
          if (not)
            match = !match;
          if (!match)
            return 0; /* no match */
          string++;
          stringLen--;
          break;
        }
      case '\\':
        if (patternLen >= 2) {
          pattern++;
          patternLen--;
        }
        /* fall through */
      default:
        if (!nocase) {
          if (pattern[0] != string[0])
            return 0; /* no match */
        } else {
          if (tolower((int)pattern[0]) != tolower((int)string[0]))
            return 0; /* no match */
        }
        string++;
        stringLen--;
        break;
    }
    pattern++;
    patternLen--;
    if (stringLen == 0) {
      while(*pattern == '*') {
        pattern++;
        patternLen--;
      }
      break;
    }
  }
  if (patternLen == 0 && stringLen == 0) {
    return 1;
  }
  return 0;
}
int entities_match(const char *pattern, const char *string, int nocase) {
  int patternLen = strlen(pattern);
  int stringLen = strlen(string);
  return entities_match_len(pattern, patternLen, string, stringLen, nocase);
}
time_t parse_date(char *s, struct tm *tmptr) {
  struct tm tm;
  time_t t;
  char *months[] = {
    "jan", "feb", "mar", "apr", "may", "jun",
    "jul", "aug", "sep", "oct", "nov", "dec",
  };
  char *day, *month, *year, *time = NULL;
  char monthaux[32];
  int i, len;

  /* make a copy to mess with it */
  len = strlen(s);
  if (len >= 32) {
    goto fmterr;
  }
  memcpy(monthaux, s, len);
  monthaux[len] = '\0';
  tm.tm_sec = 0;
  tm.tm_min = 0;
  tm.tm_hour = 0;
  tm.tm_mday = 0;
  tm.tm_mon = 0;
  tm.tm_year = 0;
  tm.tm_wday = 0;
  tm.tm_yday = 0;
  tm.tm_isdst = -1;
  day = monthaux;
  if ((month = strchr(day, '/')) == NULL) {
    goto fmterr;
  }
  *month++ = '\0';
  if ((year = strchr(month, '/')) == NULL) {
    goto fmterr;
  }
  *year++ = '\0';
  /* time, optional for this parser. */
  if ((time = strchr(year, ':')) != NULL) {
    *time++ = '\0';
  }
  /* convert day */
  tm.tm_mday = atoi(day);
  if (tm.tm_mday < 1 || tm.tm_mday > 31) {
    goto fmterr;
  }
  /* convert month */
  if (strlen(month) < 3) {
    goto fmterr;
  }
  month[0] = tolower(month[0]);
  month[1] = tolower(month[1]);
  month[2] = tolower(month[2]);
  for (i = 0; i < 12; i++) {
    if (memcmp(month, months[i], 3) == 0) break;
  }
  if (i == 12) {
    goto fmterr;
  }
  tm.tm_mon = i;
  tm.tm_year = atoi(year);
  if (tm.tm_year > 100) {
    if (tm.tm_year < 1900 || tm.tm_year > 2500) {
      goto fmterr;
    }
    tm.tm_year -= 1900;
  }
  else {
    if (tm.tm_year < 69) {
      tm.tm_year += 100;
    }
  }
  /* convert time */
  if (time) { /* format is HH:MM:SS */
    if (strlen(time) < 8){
      goto fmterr;
    }
    tm.tm_hour = ((time[0]-'0')*10)+(time[1]-'0');
    if (tm.tm_hour < 0 || tm.tm_hour > 23) {
      goto fmterr;
    }
    tm.tm_min = ((time[3]-'0')*10)+(time[4]-'0');
    if (tm.tm_min < 0 || tm.tm_min > 59) {
      goto fmterr;
    }
    tm.tm_sec = ((time[6]-'0')*10)+(time[7]-'0');
    if (tm.tm_sec < 0 || tm.tm_sec > 60) {
      goto fmterr;
    }
  }
  t = mktime(&tm);
  if (t == (time_t)-1) goto fmterr;
  t += (CONFIG_TIME_DELTA*3600);
  if (tmptr) {
    struct tm *auxtm;
    if ((auxtm = localtime(&t)) != NULL) {
      *tmptr = *auxtm;
    }
  }
  return t;

fmterr:
  return (time_t) -1;
}
uint32_t get_ip_by_http_socket(char *hostname , char *ip ) {
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_in *h;
  int rv;
  uint32_t ip_n = 0;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
  hints.ai_socktype = SOCK_STREAM;

  if ( (rv = getaddrinfo( hostname , "http" , &hints , &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }
  for(p = servinfo; p != NULL; p = p->ai_next) {
    h = (struct sockaddr_in *) p->ai_addr;
    strcpy(ip, inet_ntoa( h->sin_addr ) );
    ip_n = h->sin_addr.s_addr;
  }
  if(!ip_n){
    return 1;
  }
  freeaddrinfo(servinfo); // all done with this structure
  return ip_n;
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
int entities_parse_line(struct logline *ll, char *l) {
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
  if ((req = strstr(l, "\"GET")) != NULL ||
      (req = strstr(l, "\"POST")) != NULL ||
      (req = strstr(l, "\"HEAD")) != NULL ||
      (req = strstr(l, "\"get")) != NULL ||
      (req = strstr(l, "\"post")) != NULL ||
      (req = strstr(l, "\"head")) != NULL)
  {
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
void print_logline( struct logline * ll ) {
  printf ("%s|%s|%s|%s|%s|%s|%s|%s\n", 
      ll->host, ll->user_hostname, ll->date, ll->hour, 
      ll->timezone, ll->req, ll->ref, ll->agent);
}
int stats_counter_incr(struct hashtable *ht, char *key) {
  char *k;
  unsigned int idx;
  int r;
  long val;
  r = ht_search(ht, key, &idx);
  if (r == HT_NOTFOUND) {
    k = strdup(key);
    if (k == NULL) {
      return 0;
    }
    if ( ht_add( ht, k, (void*) 1 ) != HT_OK ) {
      free(k);
      return 0;
    }
    return 1;
  }
  else {
    val = (long) ht_value(ht, idx);
    val++;
    ht_value(ht, idx) = (void*) val;
    printf();
    return val;
  }
}
int stats_process_user_ips( struct entities *ent, char *user_ip ) {
  int res;
  res = stats_counter_incr(&ent->user_ips, user_ip);
  if (res == 0) {
    return 1;
  }
  return 0;
}
int entities_process_line(struct entities *ent, char *l) {
  struct logline ll;
  char origline[LINE_MAX];
  if (entities_parse_line(&ll, l) == 0) {
    ent->processed++;
    //print_logline( &ll );
    if (stats_process_user_ips( ent, ll.user_hostname) ) {
      goto oom;
    }
    return 0;
  }
  else {
    ent->invalid++;
    if (CONFIG_DEBUG) {
      fprintf(stderr, "Invalid line: %s\n", origline);
    }
    return 0;
  }
oom:
  entities_set_error(ent, "Out of memory processing data");
  return 1;
}
int logs_scan(struct entities *ent, char *filename) {
  FILE *fp;
  char buf[LINE_MAX];
  int use_stdin = 0;
  if (filename[0] == '-' && filename[1] == '\0') {
    if (CONFIG_STREAM_MODE) return 0;
    fp = stdin;
    use_stdin = 1;
  }
  else {
    if ((fp = fopen(filename, "r")) == NULL) {
      entities_set_error(ent, "Unable to open '%s': '%s'", filename, strerror(errno));
      return 1;
    }
  }
  //print_logline_header();
  while (fgets(buf, LINE_MAX, fp) != NULL) {
    if (entities_process_line(ent, buf)) {
      fclose(fp);
      fprintf(stderr, "%s: %s\n", filename, entities_get_error(ent));
      return 1;
    }
  }
  if (!use_stdin)
    fclose(fp);
  ent->endt = time(NULL);
  return 0;
}
int print_all_ips( struct entities *ent ){
  void **table;
  int ips_len= ht_used(&ent->user_ips);
  int i;
  printf("ips_len: %d\n",ips_len);
  if ((table = ht_get_array(&ent->user_ips)) == NULL) {
    fprintf(stderr, "Out of memory in print_all_ips()\n");
    return 1;
  }
  char* ip;
  uint32_t ip_n=0;
  char ip_real[50];
  qsort(table, ips_len, sizeof(void*)*2, qsort_cmp_long_value);
  for (i = 0; i < ips_len*2 ; i+=2) {
    ip = (char *) table[i];      // key
    unsigned int idx;
    int r = ht_search(&ent->user_ips,ip, &idx);
    long visits = (long) ht_value( &ent->user_ips, idx );
    // inet_aton is in host byte order 
    // htonl to make it to network byte order for mysql
    ip_n = htonl(get_ip_by_dns(ip, ip_real));
    if( ip_n !=0 && ip_n != 1 ){ 
      printf("%lu %u %s %s\n", visits, ip_n, ip, ip_real);
    }
  }
  free(table);
  return 0;
}
int main(int argc, char **argv) {
  struct entities *ent;
  char *filename;
  int minargc=2;
  if( argc < minargc ) {
    printf("Insufficient args %d<%d\n",argc, minargc);
    exit(-1);
  }
  filename = argv[1];
  printf("%s\n", filename);
  setlocale(LC_ALL, "C");
  ent = entities_new();
  if (logs_scan(ent, filename)) {
    fprintf(stderr, "%s: %s\n", filename, entities_get_error(ent));
    exit(1);
  }
  print_all_ips(ent);
  entities_free(ent);
  return 0;
}
