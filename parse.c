#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <locale.h>
#include <ctype.h>
#include "aht.h"
#include "sleep.h"

#define VI_ERROR_MAX 1024
#define VI_LINE_MAX 4096
#define VI_FILENAMES_MAX 1024
#define VI_PREFIXES_MAX 1024
#define VI_GREP_PATTERNS_MAX 1024
#define VI_HTML_ABBR_LEN 100
#define VI_DATE_MAX 64
#define VI_VERSION_STR "0.7"
#define VI_PATTERNTYPE_GREP 0
#define VI_PATTERNTYPE_EXCLUDE 1

struct vih {
  int startt;
  int endt;
  int processed;
  int invalid;
  int blacklisted;
  int hour[24];
  int weekday[7];
  int weekdayhour[7][24]; /* hour and weekday combined data */
  int monthday[12][31]; /* month and day combined data */
  struct hashtable visitors;
  struct hashtable googlevisitors;
  struct hashtable pages;
  struct hashtable images;
  struct hashtable error404;
  struct hashtable pageviews;
  struct hashtable pageviews_grouped;
  struct hashtable referers;
  struct hashtable referersage;
  struct hashtable date;
  struct hashtable googledate;
  struct hashtable adsensed;
  struct hashtable month;
  struct hashtable googlemonth;
  struct hashtable agents;
  struct hashtable googled;
  struct hashtable googlevisits;
  struct hashtable googlekeyphrases;
  struct hashtable googlekeyphrasesage;
  struct hashtable trails;
  struct hashtable tld;
  struct hashtable os;
  struct hashtable browsers;
  struct hashtable robots;
  struct hashtable googlehumanlanguage;
  struct hashtable screenres;
  struct hashtable screendepth;
  char *error;
};
struct logline {
  char *host;
  char *date;
  char *hour;
  char *timezone;
  char *req;
  char *ref;
  char *agent;
  time_t time;
  struct tm tm;
};
struct outputmodule {
  void (*print_header)(FILE *fp);
  void (*print_footer)(FILE *fp);
  void (*print_title)(FILE *fp, char *title);
  void (*print_subtitle)(FILE *fp, char *title);
  void (*print_numkey_info)(FILE *fp, char *key, int val);
  void (*print_keykey_entry)(FILE *fp, char *key1, char *key2, int num);
  void (*print_numkey_entry)(FILE *fp, char *key, int val, char *link, int num);
  void (*print_numkeybar_entry)(FILE *fp, char *key, int max, int tot, int this);
  void (*print_numkeycomparativebar_entry)(FILE *fp, char *key, int tot, int this);
  void (*print_bidimentional_map)(FILE *fp, int xlen, int ylen, char **xlabel, char **ylabel, int *value);
  void (*print_hline)(FILE *fp);
  void (*print_report_link)(FILE *fp, char *report);
};
struct vistring {
  char *str;
  int len;
};
struct greppat {
  int type;
  char *pattern;
};
enum { OPT_MAXREFERERS, OPT_MAXPAGES, OPT_MAXIMAGES, OPT_USERAGENTS, OPT_ALL, OPT_MAXLINES, OPT_GOOGLE, OPT_MAXGOOGLED, OPT_MAXUSERAGENTS, OPT_OUTPUT, OPT_VERSION, OPT_HELP, OPT_PREFIX, OPT_TRAILS, OPT_GOOGLEKEYPHRASES, OPT_GOOGLEKEYPHRASESAGE, OPT_MAXGOOGLEKEYPHRASES, OPT_MAXGOOGLEKEYPHRASESAGE, OPT_MAXTRAILS, OPT_GRAPHVIZ, OPT_WEEKDAYHOUR_MAP, OPT_MONTHDAY_MAP, OPT_REFERERSAGE, OPT_MAXREFERERSAGE, OPT_TAIL, OPT_TLD, OPT_MAXTLD, OPT_STREAM, OPT_OUTPUTFILE, OPT_UPDATEEVERY, OPT_RESETEVERY, OPT_OS, OPT_BROWSERS, OPT_ERROR404, OPT_MAXERROR404, OPT_TIMEDELTA, OPT_PAGEVIEWS, OPT_ROBOTS, OPT_MAXROBOTS, OPT_GRAPHVIZ_ignorenode_GOOGLE, OPT_GRAPHVIZ_ignorenode_EXTERNAL, OPT_GRAPHVIZ_ignorenode_NOREFERER, OPT_GOOGLEHUMANLANGUAGE, OPT_FILTERSPAM, OPT_MAXADSENSED, OPT_GREP, OPT_EXCLUDE, OPT_IGNORE404, OPT_DEBUG, OPT_SCREENINFO};
int Config_debug = 1;
int Config_max_referers = 2000;
int Config_max_referers_age = 30;
int Config_max_pages = 2000;
int Config_max_images = 2000;
int Config_max_error404 = 2000;
int Config_max_agents = 200;
int Config_max_googled = 2000;
int Config_max_adsensed = 2000;
int Config_max_google_keyphrases = 2000;
int Config_max_google_keyphrases_age = 2000;
int Config_max_trails = 2000;
int Config_max_tld = 2000;
int Config_max_robots = 2000;
int Config_process_agents = 1;
int Config_process_google = 1;
int Config_process_google_keyphrases = 1;
int Config_process_google_keyphrases_age = 1;
int Config_process_google_human_language = 1;
int Config_process_web_trails = 1;
int Config_process_weekdayhour_map = 1;
int Config_process_monthday_map = 1;
int Config_process_referers_age = 1;
int Config_process_tld = 1;
int Config_process_os = 1;
int Config_process_browsers = 1;
int Config_process_error404 = 1;
int Config_process_pageviews = 1;
int Config_process_monthly_visitors = 1;
int Config_process_robots = 1;
int Config_process_screen_info = 1;
int Config_graphviz_mode = 1;
int Config_graphviz_ignorenode_google = 1;
int Config_graphviz_ignorenode_external = 1;
int Config_graphviz_ignorenode_noreferer = 1;
int Config_tail_mode = 0;
int Config_stream_mode = 0;
int Config_update_every = 60*10;    /* update every 10 minutes for default. */
int Config_reset_every = 0;         /* never reset for default */
int Config_time_delta = 0;          /* adjustable time difference */
int Config_filter_spam = 0;
int Config_ignore_404 = 0;
char *Config_output_file = NULL;    /* stdout if not set. */
struct outputmodule *Output = NULL; /* intialized to 'text' in main() */
int Config_prefix_num = 0;          /* number of set prefixes */
struct vistring Config_prefix[VI_PREFIXES_MAX];
struct greppat Config_grep_pattern[VI_GREP_PATTERNS_MAX];
int Config_grep_pattern_num = 0;    /* number of set patterns */
static char *vi_wdname[7] = {"Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"};
#if 0
static int vi_monthdays[12] = {31, 29, 31, 30, 31, 30 , 31, 31, 30, 31, 30, 31};
#endif
void vi_clear_error(struct vih *vih);
void vi_reset_combined_maps(struct vih *vih) {
  int i, j;

  for (i = 0; i < 24; i++) {
    vih->hour[i] = 0;
    for (j = 0; j < 7; j++)
      vih->weekdayhour[j][i] = 0;
  }
  for (i = 0; i < 7; i++) vih->weekday[i] = 0;
  for (i = 0; i < 31; i++)
    for (j = 0; j < 12; j++)
      vih->monthday[j][i] = 0;
}
void vi_ht_init(struct hashtable *ht) {
  ht_init(ht);
  ht_set_hash(ht, ht_hash_string);
  ht_set_key_destructor(ht, ht_destructor_free);
  ht_set_val_destructor(ht, ht_no_destructor);
  ht_set_key_compare(ht, ht_compare_string);
}
struct vih *vi_new(void) {
  struct vih *vih;

  if ((vih = malloc(sizeof(*vih))) == NULL)
    return NULL;
  /* Initialization */
  vih->startt = vih->endt = time(NULL);
  vih->processed = 0;
  vih->invalid = 0;
  vih->blacklisted = 0;
  vi_reset_combined_maps(vih);
  vih->error = NULL;
  vi_ht_init(&vih->visitors);
  vi_ht_init(&vih->googlevisitors);
  vi_ht_init(&vih->pages);
  vi_ht_init(&vih->images);
  vi_ht_init(&vih->error404);
  vi_ht_init(&vih->pageviews);
  vi_ht_init(&vih->pageviews_grouped);
  vi_ht_init(&vih->referers);
  vi_ht_init(&vih->referersage);
  vi_ht_init(&vih->agents);
  vi_ht_init(&vih->googled);
  vi_ht_init(&vih->adsensed);
  vi_ht_init(&vih->googlevisits);
  vi_ht_init(&vih->googlekeyphrases);
  vi_ht_init(&vih->googlekeyphrasesage);
  vi_ht_init(&vih->trails);
  vi_ht_init(&vih->tld);
  vi_ht_init(&vih->os);
  vi_ht_init(&vih->browsers);
  vi_ht_init(&vih->date);
  vi_ht_init(&vih->month);
  vi_ht_init(&vih->googledate);
  vi_ht_init(&vih->googlemonth);
  vi_ht_init(&vih->robots);
  vi_ht_init(&vih->googlehumanlanguage);
  vi_ht_init(&vih->screenres);
  vi_ht_init(&vih->screendepth);
  return vih;
}
void vi_set_error(struct vih *vih, char *fmt, ...) {
  va_list ap;
  char buf[VI_ERROR_MAX];

  va_start(ap, fmt);
  vsnprintf(buf, VI_ERROR_MAX, fmt, ap);
  buf[VI_ERROR_MAX-1] = '\0';
  free(vih->error);
  vih->error = strdup(buf);
  va_end(ap);
}
void vi_reset_hashtables(struct vih *vih) {
  ht_destroy(&vih->visitors);
  ht_destroy(&vih->googlevisitors);
  ht_destroy(&vih->pages);
  ht_destroy(&vih->images);
  ht_destroy(&vih->error404);
  ht_destroy(&vih->pageviews);
  ht_destroy(&vih->pageviews_grouped);
  ht_destroy(&vih->referers);
  ht_destroy(&vih->referersage);
  ht_destroy(&vih->agents);
  ht_destroy(&vih->googled);
  ht_destroy(&vih->adsensed);
  ht_destroy(&vih->googlekeyphrases);
  ht_destroy(&vih->googlekeyphrasesage);
  ht_destroy(&vih->googlevisits);
  ht_destroy(&vih->trails);
  ht_destroy(&vih->tld);
  ht_destroy(&vih->os);
  ht_destroy(&vih->browsers);
  ht_destroy(&vih->date);
  ht_destroy(&vih->googledate);
  ht_destroy(&vih->month);
  ht_destroy(&vih->googlemonth);
  ht_destroy(&vih->robots);
  ht_destroy(&vih->googlehumanlanguage);
  ht_destroy(&vih->screenres);
  ht_destroy(&vih->screendepth);
}
char *vi_get_error(struct vih *vih) {
  if (!vih->error) {
    return "No error";
  }
  return vih->error;
}
void vi_clear_error(struct vih *vih) {
  free(vih->error);
  vih->error = NULL;
}
void vi_free(struct vih *vih) {
  if (!vih) return;
  vi_reset_hashtables(vih);
  vi_clear_error(vih);
  free(vih);
}
int vi_match_len(const char *pattern, int patternLen, const char *string, int stringLen, int nocase) {
  while(patternLen) {
    switch(pattern[0]) {
      case '*':
        while (pattern[1] == '*') {
          pattern++;
          patternLen--;
        }
        if (patternLen == 1)
          return 1; /* match */
        while(stringLen) {
          if (vi_match_len(pattern+1, patternLen-1,
                string, stringLen, nocase))
            return 1; /* match */
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
  if (patternLen == 0 && stringLen == 0)
    return 1;
  return 0;
}
int vi_match(const char *pattern, const char *string, int nocase) {
  int patternLen = strlen(pattern);
  int stringLen = strlen(string);
  return vi_match_len(pattern, patternLen, string, stringLen, nocase);
}
int vi_match_line(char *line) {
  int i;

  for (i = 0; i < Config_grep_pattern_num; i++) {
    char *pattern = Config_grep_pattern[i].pattern;
    int nocase = 1;
    if (pattern[0] == 'c' && pattern[1] == 's' && pattern[2] == ':') {
      nocase = 0;
      pattern += 3;
    }
    if (vi_match(Config_grep_pattern[i].pattern, line, nocase)) {
      if (Config_grep_pattern[i].type == VI_PATTERNTYPE_EXCLUDE)
        return 0;
    } else {
      if (Config_grep_pattern[i].type == VI_PATTERNTYPE_GREP)
        return 0;
    }
  }
  return 1;
}
int vi_strlcpy(char *dst, char *src, int siz) {
  char *d = dst;
  const char *s = src;
  int n = siz;

  if (n != 0 && --n != 0) {
    do {
      if ((*d++ = *s++) == 0)
        break;
    } while (--n != 0);
  }

  if (n == 0) {
    if (siz != 0)
      *d = '\0';
    while (*s++)
      ;
  }
  return(s - src - 1);
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
  if (len >= 32) goto fmterr;
  memcpy(monthaux, s, len);
  monthaux[len] = '\0';

  /* Inizialize the tm structure. We just fill three fields */
  tm.tm_sec = 0;
  tm.tm_min = 0;
  tm.tm_hour = 0;
  tm.tm_mday = 0;
  tm.tm_mon = 0;
  tm.tm_year = 0;
  tm.tm_wday = 0;
  tm.tm_yday = 0;
  tm.tm_isdst = -1;

  /* search delimiters */
  day = monthaux;
  if ((month = strchr(day, '/')) == NULL) goto fmterr;
  *month++ = '\0';
  if ((year = strchr(month, '/')) == NULL) goto fmterr;
  *year++ = '\0';
  /* time, optional for this parser. */
  if ((time = strchr(year, ':')) != NULL) {
    *time++ = '\0';
  }
  /* convert day */
  tm.tm_mday = atoi(day);
  if (tm.tm_mday < 1 || tm.tm_mday > 31) goto fmterr;
  /* convert month */
  if (strlen(month) < 3) goto fmterr;
  month[0] = tolower(month[0]);
  month[1] = tolower(month[1]);
  month[2] = tolower(month[2]);
  for (i = 0; i < 12; i++) {
    if (memcmp(month, months[i], 3) == 0) break;
  }
  if (i == 12) goto fmterr;
  tm.tm_mon = i;
  /* convert year */
  tm.tm_year = atoi(year);
  if (tm.tm_year > 100) {
    if (tm.tm_year < 1900 || tm.tm_year > 2500) goto fmterr;
    tm.tm_year -= 1900;
  } else {
    /* if the year is in two-digits form, the 0 - 68 range
     * is converted to 2000 - 2068 */
    if (tm.tm_year < 69)
      tm.tm_year += 100;
  }
  /* convert time */
  if (time) { /* format is HH:MM:SS */
    if (strlen(time) < 8) goto fmterr;
    tm.tm_hour = ((time[0]-'0')*10)+(time[1]-'0');
    if (tm.tm_hour < 0 || tm.tm_hour > 23) goto fmterr;
    tm.tm_min = ((time[3]-'0')*10)+(time[4]-'0');
    if (tm.tm_min < 0 || tm.tm_min > 59) goto fmterr;
    tm.tm_sec = ((time[6]-'0')*10)+(time[7]-'0');
    if (tm.tm_sec < 0 || tm.tm_sec > 60) goto fmterr;
  }
  t = mktime(&tm);
  if (t == (time_t)-1) goto fmterr;
  t += (Config_time_delta*3600);
  if (tmptr) {
    struct tm *auxtm;

    if ((auxtm = localtime(&t)) != NULL)
      *tmptr = *auxtm;
  }
  return t;

fmterr: /* format error */
  return (time_t) -1;
}
int vi_parse_line(struct logline *ll, char *l) {
  char *date, *hour, *timezone, *host, *agent, *req, *ref, *p;
  char *agent_start = NULL, *req_end = NULL, *ref_end = NULL;
  int agent_without_parens = 0;
  host = l;
  if ((date = strchr(l, '[')) == NULL) return 1;
  date++;
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
  } else {
    p = agent;
    while (p >= l) {
      if (*p == '"') {
        agent_start = p;
        break;
      }
      p--;
    }
  }
  if ((req = strstr(l, "\"GET")) != NULL ||
      (req = strstr(l, "\"POST")) != NULL ||
      (req = strstr(l, "\"HEAD")) != NULL ||
      (req = strstr(l, "\"get")) != NULL ||
      (req = strstr(l, "\"post")) != NULL ||
      (req = strstr(l, "\"head")) != NULL)
  {
    req++;
  } else {
    req = "";
  }
  if ((ref = strstr(l, "\"http")) != NULL ||
      (ref = strstr(l, "\"HTTP")) != NULL)
  {
    ref++;
  } else {
    ref = "";
  }
  if ((p = strchr(host, ' ')) == NULL) {
    return 1;
  }
  *p = '\0';
  if ((p = strchr(date, ']')) == NULL) {
    return 1;
  }
  *p = '\0';
  ll->time = parse_date(date, &ll->tm);
  if (ll->time == (time_t)-1) {
    return 1;
  }
  if ((p = strchr(date, ':')) == NULL) {
    return 1;
  }
  hour = p+1;
  *p = '\0';
  if ((p = strchr(hour, ' ')) == NULL) {
    return 1;
  }
  timezone = p+1;
  *p = '\0';
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
  if ((p = strchr(ref, '"')) == NULL) {
    ref = "";
  }
  else {
    ref_end = p;
    *p = '\0';
  }
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
  ll->date = date;
  ll->hour = hour;
  ll->timezone = timezone;
  ll->agent = agent;
  ll->req = req;
  ll->ref = ref;
  return 0;
}
void vi_urldecode(char *d, char *s, int n) {
  char *start = d;
  if (n < 1) return;
  while(*s && n > 1) {
    int c = *s;
    switch(c) {
      case '+': c = ' '; break;
      case '%':
                if (*(s+1) && *(s+2)) {
                  int high = toupper(*(s+1));
                  int low = toupper(*(s+2));

                  if (high <= '9') high -= '0';
                  else high = (high - 'A') + 10;
                  if (low <= '9') low -= '0';
                  else low = (low - 'A') + 10;
                  c = (high << 4)+low;
                  s += 2;
                }
                break;
    }
    if (c != ' ' || d != start) {
      *d++ = c;
      n--;
    }
    s++;
  }
  /* Right trim */
  *d = '\0';
  d--;
  while (d >= start && *d == ' ') {
    *d = '\0';
    d--;
  }
}
int vi_counter_incr(struct hashtable *ht, char *key) {
  char *k;
  unsigned int idx;
  int r;
  long val;

  r = ht_search(ht, key, &idx);
  if (r == HT_NOTFOUND) {
    k = strdup(key);
    if (k == NULL) return 0;
    if (ht_add(ht, k, (void*)1) != HT_OK) {
      free(k);
      return 0;
    }
    return 1;
  } else {
    val = (long) ht_value(ht, idx);
    val++;
    ht_value(ht, idx) = (void*) val;
    return val;
  }
}
int vi_process_error404(struct vih *vih, char *l, char *url, int *is404) {
  char urldecoded[VI_LINE_MAX];

  if (is404) *is404 = 0;
  vi_urldecode(urldecoded, url, VI_LINE_MAX);
  if (strstr(l, " 404 ") && !strstr(l, " 200 ")) {
    if (is404) *is404 = 1;
    return !vi_counter_incr(&vih->error404, urldecoded);
  }
  return 0;
}
int vi_process_screen_info(struct vih *vih, char *req) {
  char *p;

  if ((p = strstr(req, "visitors-screen-res-check.jpg?"))) {
    char buf[64];

    p += 30;
    if (p[0] == '\0' || strstr(p, "undefined")) goto parseerror;
    vi_strlcpy(buf, p, 64);
    /* The string is somethink like: 1024x768x32, so we
     * search for the second 'x' char. */
    p = strchr(buf,'x'); if (!p) goto parseerror;
    p = strchr(p+1,'x'); if (!p) goto parseerror;
    *p = '\0'; p++;
    /* Populate the screen resolution hash table */
    if (vi_counter_incr(&vih->screenres, buf) == 0)
      return 1;
    /* ... and the screen color depth one. */
    if (vi_counter_incr(&vih->screendepth, p) == 0)
      return 1;
  }
parseerror:
  return 0;
}
int vi_is_googlebot_agent(char *agent) {
  if (strstr(agent, "Googlebot") ||
      strstr(agent, "googlebot")) return 1;
  return 0;
}
int vi_is_adsensebot_agent(char *agent) {
  if (strstr(agent, "Mediapartners-Google")) return 1;
  return 0;
}
int vi_is_yahoobot_agent(char *agent) {
  if (strstr(agent, "Yahoo! Slurp")) return 1;
  return 0;
}
int vi_is_msbot_agent(char *agent) {
  if (strstr(agent, "msn.com/msnbot.htm")) return 1;
  return 0;
}
int vi_is_bot_agent(char *agent) {
  if (vi_is_googlebot_agent(agent) ||
      vi_is_adsensebot_agent(agent) ||
      vi_is_yahoobot_agent(agent) ||
      vi_is_msbot_agent(agent)) return 1;
  return 0;
}
int vi_is_google_link(char *s) {
  return !strncmp(s, "http://www.google.", 18);
}
int vi_is_pageview(char *s) {
  int l = strlen(s);
  char *end = s + l; /* point to the nul term */
  char *dot, *slash;

  if (s[l-1] == '/') return 1;
  if (l >= 6 &&
      (!memcmp(end-5, ".html", 5) || 
       !memcmp(end-4, ".htm", 4) || 
       !memcmp(end-4, ".php", 4) ||
       !memcmp(end-4, ".asp", 4) ||
       !memcmp(end-4, ".jsp", 4) ||
       !memcmp(end-4, ".xdl", 4) ||
       !memcmp(end-5, ".xhtml", 5) ||
       !memcmp(end-4, ".xml", 4) ||
       !memcmp(end-4, ".cgi", 4) ||
       !memcmp(end-3, ".pl", 3) ||
       !memcmp(end-6, ".shtml", 6) ||
       !memcmp(end-5, ".HTML", 5) || 
       !memcmp(end-4, ".HTM", 4) || 
       !memcmp(end-4, ".PHP", 4) ||
       !memcmp(end-4, ".ASP", 4) ||
       !memcmp(end-4, ".JSP", 4) ||
       !memcmp(end-4, ".XDL", 4) ||
       !memcmp(end-6, ".XHTML", 6) ||
       !memcmp(end-4, ".XML", 4) ||
       !memcmp(end-4, ".CGI", 4) ||
       !memcmp(end-3, ".PL", 3) ||
       !memcmp(end-6, ".SHTML", 6))) return 1;
  dot = strrchr(s, '.');
  if (!dot) return 1;
  slash = strrchr(s, '/');
  if (slash && slash > dot) return 1;
  return 0;
}
int vi_process_visitors_per_day(struct vih *vih, char *host, char *agent, char *date, char *ref, char *req, int *seen) {
  char visday[VI_LINE_MAX], *p, *month = "fixme if I'm here!";
  char buf[64];
  int res, host_len, agent_len, date_len, hash_len;
  unsigned long h;

  if (vi_is_bot_agent(agent)) {
    if (seen != NULL) seen = 0;
    return 0;
  }
  host_len = strlen(host);
  agent_len = strlen(agent);
  date_len = strlen(date);
  h = djb_hash((unsigned char*) agent, agent_len);
  sprintf(buf, "%lu", h);
  hash_len = strlen(buf);
  if (host_len+agent_len+date_len+4 > VI_LINE_MAX)
    return 0;
  p = visday;
  memcpy(p, host, host_len); p += host_len;
  *p++ = '|';
  memcpy(p, date, date_len); p += date_len;
  *p++ = '|';
  memcpy(p, buf, hash_len); p += hash_len;
  *p = '\0';
  if (Config_process_monthly_visitors) {
    month = strchr(date, '/');
    if (!month) return 0;
    month++;
  }
  if (vi_is_google_link(ref)) {
    res = vi_counter_incr(&vih->googlevisitors, visday);
    if (res == 0) return 1; /* out of memory */
    if (res == 1) { /* new visit! */
      res = vi_counter_incr(&vih->googledate, date);
      if (res == 0) return 1; /* out of memory */
      if (Config_process_monthly_visitors) {
        res = vi_counter_incr(&vih->googlemonth, month);
        if (res == 0) return 1; /* out of memory */
      }
    }
  }
  if (Config_process_pageviews && vi_is_pageview(req)) {
    res = vi_counter_incr(&vih->pageviews, visday);
    if (res == 0) return 1; /* out of memory */
  }
  res = vi_counter_incr(&vih->visitors, visday);
  if (res == 0) return 1; /* out of memory */
  if (res > 1) {
    if (seen) *seen = 1;
    return 0; /* visit alredy seen. */
  }
  if (seen) *seen = 0; /* new visitor */
  res = vi_counter_incr(&vih->date, date);
  if (res == 0) return 1;
  if (Config_process_monthly_visitors) {
    res = vi_counter_incr(&vih->month, month);
    if (res == 0) return 1;
  }
  return 0;
}
int vi_is_image(char *s) {
  int l = strlen(s);
  char *end = s + l; /* point to the nul term */

  if (l < 5) return 0;
  if (!memcmp(end-4, ".css", 4) || 
      !memcmp(end-4, ".jpg", 4) || 
      !memcmp(end-4, ".gif", 4) ||
      !memcmp(end-4, ".png", 4) ||
      !memcmp(end-4, ".ico", 4) ||
      !memcmp(end-4, ".swf", 4) ||
      !memcmp(end-3, ".js", 3) ||
      !memcmp(end-5, ".jpeg", 5) ||
      !memcmp(end-4, ".CSS", 4) ||
      !memcmp(end-4, ".JPG", 4) ||
      !memcmp(end-4, ".GIF", 4) ||
      !memcmp(end-4, ".PNG", 4) ||
      !memcmp(end-4, ".ICO", 4) ||
      !memcmp(end-4, ".SWF", 4) ||
      !memcmp(end-3, ".JS", 3) ||
      !memcmp(end-5, ".JPEG", 5)) return 1;
  return 0;
}
int vi_process_page_request(struct vih *vih, char *url) {
  int res;
  char urldecoded[VI_LINE_MAX];

  vi_urldecode(urldecoded, url, VI_LINE_MAX);
  if (vi_is_image(url))
    res = vi_counter_incr(&vih->images, urldecoded);
  else
    res = vi_counter_incr(&vih->pages, urldecoded);
  if (res == 0) return 1;
  return 0;
}
int vi_replace_time(struct hashtable *ht, char *key, time_t time, int ifolder) {
  char *k = NULL;
  unsigned int idx;
  int r;

  r = ht_search(ht, key, &idx);
  if (r == HT_NOTFOUND) {
    k = strdup(key);
    if (!k) goto err;
    if (ht_add(ht, k, (void*)time) != HT_OK) goto err;
  } else {
    time_t oldt = (time_t) ht_value(ht, idx);
    /* Update the date if this one is older/nwer. */
    if (ifolder) {
      if (time < oldt)
        ht_value(ht, idx) = (void*) time;
    } else {
      if (time > oldt)
        ht_value(ht, idx) = (void*) time;
    }
  }
  return 0;
err:
  if (k) free(k);
  return 1;
}
int vi_replace_if_newer(struct hashtable *ht, char *key, time_t time) {
  return vi_replace_time(ht, key, time, 0);
}
int vi_process_googled(struct vih *vih, char *req, char *agent, time_t age) {
  if (vi_is_googlebot_agent(agent)) {
    return vi_replace_if_newer(&vih->googled, req, age);
  } else if (vi_is_adsensebot_agent(agent)) {
    return vi_replace_if_newer(&vih->adsensed, req, age);
  }
  return 0;
}
int vi_is_internal_link(char *url) {
  int i, l;

  if (!Config_prefix_num) return 0; /* no prefixes set? */
  l = strlen(url);
  for (i = 0; i < Config_prefix_num; i++) {
    if (Config_prefix[i].len <= l &&
        !strncasecmp(url, Config_prefix[i].str,
          Config_prefix[i].len))
    {
      return Config_prefix[i].len;
    }
  }
  return 0;
}
int vi_process_web_trails(struct vih *vih, char *ref, char *req) {
  int res, plen, google;
  char buf[VI_LINE_MAX];
  char *src;

  if (vi_is_image(req)) return 0;
  plen = vi_is_internal_link(ref);
  google = vi_is_google_link(ref);
  if (plen) {
    src = (ref[plen] == '\0') ? "/" : ref+plen;
  } else if (google) {
    if (Config_graphviz_ignorenode_google) return 0;
    src = "Google";
  } else if (ref[0] != '\0') {
    if (Config_graphviz_ignorenode_external) return 0;
    src = "External Link";
  } else {
    if (Config_graphviz_ignorenode_noreferer) return 0;
    src = "No Referer";
  }
  if (!strcmp(src, req)) return 0; /* avoid self references */

  snprintf(buf, VI_LINE_MAX, "%s -> %s", src, req);
  buf[VI_LINE_MAX-1] = '\0';
  res = vi_counter_incr(&vih->trails, buf);
  if (res == 0) return 1;
  return 0;
}
void vi_strtolower(char *s) {
  while (*s) {
    *s = tolower(*s);
    s++;
  }
}
int vi_strlcat(char *dst, const char *src, int siz) {
  char *d = dst;
  const char *s = src;
  size_t n = siz;
  size_t dlen;

  /* Find the end of dst and adjust bytes left but don't go past end */
  while (n-- != 0 && *d != '\0')
    d++;
  dlen = d - dst;
  n = siz - dlen;

  if (n == 0)
    return(dlen + strlen(s));
  while (*s != '\0') {
    if (n != 1) {
      *d++ = *s;
      n--;
    }
    s++;
  }
  *d = '\0';

  return(dlen + (s - src));       /* count does not include NUL */
}
int vi_replace_if_older(struct hashtable *ht, char *key, time_t time) {
  return vi_replace_time(ht, key, time, 1);
}
int vi_process_google_keyphrases(struct vih *vih, char *ref, time_t age) {
  char *s, *p, *e;
  int res, page;
  char urldecoded[VI_LINE_MAX];
  char buf[64];
  if (!vi_is_google_link(ref)){ 
    return 0;
  }
  if (Config_process_google_human_language) {
    s = strstr(ref+18, "&hl=");
    if (s == NULL) s = strstr(ref+18, "?hl=");
    if (s && s[4] && s[5]) {
      buf[0] = s[4];
      buf[1] = s[5];
      buf[2] = '\0';
      if (vi_counter_incr(&vih->googlehumanlanguage, buf) == 0)
        return 1;
    }
  }
  if ((s = strstr(ref+18, "?q=")) == NULL && (s = strstr(ref+18, "&q=")) == NULL) {
    return 0;
  }
  if ((p = strstr(ref+18, "&start=")) == NULL) {
    p = strstr(ref+18, "?start=");
  }
  if ((e = strchr(s+3, '&')) != NULL) {
    *e = '\0';
  }
  if (p && (e = strchr(p+7, '&')) != NULL) {
    *e = '\0';
  }
  if (!strncmp(s+3, "cache:", 6)) {
    return !vi_counter_incr(&vih->googlekeyphrases, "Google Cache Access");
  }
  vi_urldecode(urldecoded, s+3, VI_LINE_MAX);
  vi_strtolower(urldecoded);
  page = p ? (1+(atoi(p+7)/10)) : 1;
  snprintf(buf, 64, " (page %d)", page);
  buf[63] = '\0';
  vi_strlcat(urldecoded, buf, VI_LINE_MAX);
  res = vi_counter_incr(&vih->googlekeyphrases, urldecoded);
  if (e) *e = '&';
  if (res == 0) return 1;
  if (Config_process_google_keyphrases_age) {
    if (vi_replace_if_older(&vih->googlekeyphrasesage, urldecoded, age)) {
      return 1;
    }
  }
  return 0;
}
void vi_process_date_and_hour(struct vih *vih, int weekday, int hour) {
  /* Note, the following sanity check is useless in theory. */
  if (weekday < 0 || weekday > 6 || hour < 0 || hour > 23) return;
  vih->weekday[weekday]++;
  vih->hour[hour]++;
  /* store the combined info. We always compute this information
   * even if the report is disabled because it's cheap. */
  vih->weekdayhour[weekday][hour]++;
}
void vi_process_month_and_day(struct vih *vih, int month, int day) {
  if (month < 0 || month > 11 || day < 0 || day > 30) return;
  vih->monthday[month][day]++;
}
/*
int vi_is_blacklisted_url(struct vih *vih, char *url) {
  unsigned int i;

  for (i = 0; i < VI_BLACKLIST_LEN; i++) {
    if (strstr(url, vi_blacklist[i])) {
      vih->blacklisted++;
      return 1;
    }
  }
  return 0;
}
*/
int vi_process_referer(struct vih *vih, char *ref, time_t age) {
  int res;
  /* Check the url against the blacklist if needed
   * this can be very slow... */
  //if (Config_filter_spam && vi_is_blacklisted_url(vih, ref))
  //  return 0;
  /* Don't count internal referer (specified by the user
   * using --prefix options), nor google referers. */
  if (vi_is_internal_link(ref))
    return !vi_counter_incr(&vih->referers, "Internal Link");
  if (vi_is_google_link(ref))
    return !vi_counter_incr(&vih->referers, "Google Search Engine");
  res = vi_counter_incr(&vih->referers, ref);
  if (res == 0) return 1;
  /* Process the referers age if enabled */
  if (Config_process_referers_age) {
    if (vi_replace_if_older(&vih->referersage, ref, age)) return 1;
  }
  return 0;
}
int vi_process_agents(struct vih *vih, char *agent) {
  int res;

  res = vi_counter_incr(&vih->agents, agent);
  if (res == 0) return 1;
  return 0;
}
int vi_counter_incr_matchtable(struct hashtable *ht, char *s, char **t) {
  while(*t) {
    int res;
    if ((*t)[0] == '\0' || strstr(s, *t) != NULL) {
      char *key = *(t+1) ? *(t+1) : *t;
      res = vi_counter_incr(ht, key);
      if (res == 0) return 1;
      return 0;
    }
    t += 2;
  }
  return 0;
}
int vi_process_os(struct vih *vih, char *agent) {
  /* Order may matter. */
  char *oslist[] = {
    "Windows", NULL,
    "Win98", "Windows",
    "Win95", "Windows",
    "WinNT", "Windows",
    "Win32", "Windows",
    "Linux", NULL,
    "-linux-", "Linux",
    "Macintosh", NULL,
    "Mac_PowerPC", "Macintosh",
    "SunOS", NULL,
    "FreeBSD", NULL,
    "OpenBSD", NULL,
    "NetBSD", NULL,
    "BEOS", NULL,
    "", "Unknown",
    NULL, NULL,
  };
  return vi_counter_incr_matchtable(&vih->os, agent, oslist);
}
int vi_process_browsers(struct vih *vih, char *agent) {
  /* Note that the order matters. For example Safari
   * send an user agent where there is the string "Gecko"
   * so it must be before Gecko. */
  char *browserslist[] = {
    "Opera", NULL,
    "MSIE 4", "Explorer 4.x",
    "MSIE 5", "Explorer 5.x",
    "MSIE 6", "Explorer 6.x",
    "MSIE", "Explorer unknown version",
    "Safari", NULL,
    "Konqueror", NULL,
    "Galeon", NULL,
    "Firefox", NULL,
    "MultiZilla", NULL,
    "Gecko", "Other Mozilla based",
    "Wget", NULL,
    "Lynx", NULL,
    "Links ", "Links",
    "ELinks ", "Links",
    "Elinks ", "Links",
    "w3m", "W3M",
    "NATSU-MICAN", NULL,
    "msnbot", "MSNbot",
    "Slurp", "Yahoo Slurp",
    "Jeeves", "Ask Jeeves",
    "ZyBorg", NULL,
    "asteria", NULL,
    "contype", "Explorer",
    "Gigabot", NULL,
    "Windows-Media-Player", "Windows-MP",
    "NSPlayer", NULL,
    "Googlebot", "GoogleBot",
    "googlebot", "GoogleBot",
    "", "Unknown",
    NULL, NULL,
  };
  return vi_counter_incr_matchtable(&vih->browsers, agent, browserslist);
}
int vi_is_numeric_address(char *ip) {
  unsigned int l = strlen(ip);
  return strspn(ip, "0123456789.") == l;
}
int vi_process_tld(struct vih *vih, char *hostname) {
  char *tld;
  int res;
  if (vi_is_numeric_address(hostname)) {
    tld = "numeric IP";
  } else {
    tld = strrchr(hostname, '.');
    if (!tld) return 0;
    tld++;
  }
  res = vi_counter_incr(&vih->tld, tld);
  if (res == 0) return 1;
  return 0;
}
int vi_process_robots(struct vih *vih, char *req, char *agent) {
  if (strncmp(req, "/robots.txt", 11) != 0) return 0;
  if (strstr(agent, "MSIECrawler")) return 0;
  return !vi_counter_incr(&vih->robots, agent);
}
int vi_process_line(struct vih *vih, char *l) {
  struct logline ll;
  char origline[VI_LINE_MAX];
  if (Config_grep_pattern_num) {
    if (vi_match_line(l) == 0)
      return 0; /* No match? skip. */
  }
  vih->processed++;
  if (Config_process_error404 || Config_debug) {
    vi_strlcpy(origline, l, VI_LINE_MAX);
  }
  if (vi_parse_line(&ll, l) == 0) {
    int seen, is404;
    if (Config_process_error404 && vi_process_error404(vih, origline, ll.req, &is404)) {
      goto oom;
    }
    if (Config_process_screen_info && is404) {
      if (vi_process_screen_info(vih, ll.req)) {
        goto oom;
      }
    }
    if (Config_ignore_404 && is404) {
      return 0;
    }
    if (vi_process_visitors_per_day(vih, ll.host, ll.agent, ll.date, ll.ref, ll.req, &seen)){
      goto oom;
    }
    if (vi_process_page_request(vih, ll.req)) {
      goto oom;
    }

    if (Config_process_google && vi_process_googled(vih, ll.req, ll.agent, ll.time)) {
      goto oom;
    }
    if (Config_process_web_trails && vi_process_web_trails(vih, ll.ref, ll.req)) {
      goto oom;
    }
    if (Config_process_google_keyphrases && vi_process_google_keyphrases(vih, ll.ref, ll.time)) {
      goto oom;
    }
    if (seen) {
      return 0;
    }
    vi_process_date_and_hour(vih, (ll.tm.tm_wday+6)%7, ll.tm.tm_hour);
    vi_process_month_and_day(vih, ll.tm.tm_mon, ll.tm.tm_mday-1);
    if (vi_process_referer(vih, ll.ref, ll.time)) {
      goto oom;
    }
    if (Config_process_agents && vi_process_agents(vih, ll.agent)) {
      goto oom;
    }
    if (Config_process_os && vi_process_os(vih, ll.agent)) {
      goto oom;
    }
    if (Config_process_browsers && vi_process_browsers(vih, ll.agent)) {
      goto oom;
    }
    if (Config_process_tld && vi_process_tld(vih, ll.host)) {
      goto oom;
    }
    if (Config_process_robots && vi_process_robots(vih, ll.req, ll.agent)) {
      goto oom;
    }
    return 0;
  } else {
    vih->invalid++;
    if (Config_debug) {
      fprintf(stderr, "Invalid line: %s\n", origline);
    }
    return 0;
  }
oom:
  vi_set_error(vih, "Out of memory processing data");
  return 1;
}
int vi_scan(struct vih *vih, char *filename) {
  FILE *fp;
  char buf[VI_LINE_MAX];
  int use_stdin = 0;
  if (filename[0] == '-' && filename[1] == '\0') {
    if (Config_stream_mode) return 0;
    fp = stdin;
    use_stdin = 1;
  } else {
    if ((fp = fopen(filename, "r")) == NULL) {
      vi_set_error(vih, "Unable to open '%s': '%s'", filename, strerror(errno));
      return 1;
    }
  }
  while (fgets(buf, VI_LINE_MAX, fp) != NULL) {
    if (vi_process_line(vih, buf)) {
      fclose(fp);
      fprintf(stderr, "%s: %s\n", filename, vi_get_error(vih));
      return 1;
    }
  }
  if (!use_stdin)
    fclose(fp);
  vih->endt = time(NULL);
  return 0;
}
void om_text_print_header(FILE *fp) {
  fp = fp;
  return;
}
void om_text_print_footer(FILE *fp) {
  fp = fp;
  return;
}
void om_text_print_title(FILE *fp, char *title) {
  fprintf(fp, "=== %s ===\n", title);
}
void om_text_print_subtitle(FILE *fp, char *subtitle) {
  fprintf(fp, "--- %s\n", subtitle);
}
void om_text_print_numkey_info(FILE *fp, char *key, int val) {
  fprintf(fp, "* %s: %d\n", key, val);
}
void om_text_print_keykey_entry(FILE *fp, char *key1, char *key2, int num) {
  fprintf(fp, "%d)    %s: %s\n", num, key1, key2);
}
void om_text_print_numkey_entry(FILE *fp, char *key, int val, char *link,
    int num) {
  link = link; /* avoid warning. Text output don't use this argument. */
  fprintf(fp, "%d)    %s: %d\n", num, key, val);
}
void om_text_print_bar(FILE *fp, int max, int tot, int this, int cols, char c1, char c2) {
  int l;
  float p;
  char *bar;
  if (tot == 0) tot++;
  if (max == 0) max++;
  l = ((float)(cols*this))/max;
  p = ((float)(100*this))/tot;
  bar = malloc(cols+1);
  if (!bar) return;
  memset(bar, c2, cols+1);
  memset(bar, c1, l);
  bar[cols] = '\0';
  fprintf(fp, "%s %02.1f%%", bar, p);
  free(bar);
}
void om_text_print_numkeybar_entry(FILE *fp, char *key, int max, int tot, int this) {
  fprintf(fp, "   %-12s: %-9d |", key, this);
  om_text_print_bar(fp, max, tot, this, 44, '#', ' ');
  fprintf(fp, "\n");
}
void om_text_print_numkeycomparativebar_entry(FILE *fp, char *key, int tot, int this) {
  fprintf(fp, "   %s: %-10d |", key, this);
  om_text_print_bar(fp, tot, tot, this, 44, '#', '.');
  fprintf(fp, "\n");
}
void om_text_print_bidimentional_map(FILE *fp, int xlen, int ylen, char **xlabel, char **ylabel, int *value) {
  char *asciipal = " .-+#";
  int pallen = strlen(asciipal);
  int x, y, l, max = 0;
  l = xlen*ylen;
  for (x = 0; x < l; x++)
    if (max < value[x])
      max = value[x];
  if (max == 0) max++; /* avoid division by zero */
  for (y = 0; y < ylen; y++) {
    fprintf(fp, "%15s: ", ylabel[y]);
    for (x = 0; x < xlen; x++) {
      int coloridx;
      int val = value[(y*xlen)+x];

      coloridx = ((pallen-1)*val)/max;
      fputc(asciipal[coloridx], fp);
    }
    fprintf(fp, "\n");
  }
  fprintf(fp, "\n");
  {
    char **p = malloc(sizeof(char*)*xlen);
    /* The 'p' pointers array is initialized at the
     * start of all the x-labels. */
    for (x = 0; x < xlen; x++)
      p[x] = xlabel[x];
    while(1) {
      int sentinel = 0;
      fprintf(fp, "%15s  ", "");
      for (x = 0; x < xlen; x++) {
        if (*(p[x]) != '\0') {
          fputc(*(p[x]), fp);
          p[x]++;
          sentinel++;
        } else {
          fputc(' ', fp);
        }
      }
      fputc('\n', fp);
      if (sentinel == 0) break;
    }
    free(p);
  }
}
void om_text_print_hline(FILE *fp) {
  fprintf(fp, "\n");
}
void om_text_print_report_link(FILE *fp, char *report) {
  fprintf(fp, "-> %s\n", report);
  return;
}
struct outputmodule OutputModuleText = {
  om_text_print_header,
  om_text_print_footer,
  om_text_print_title,
  om_text_print_subtitle,
  om_text_print_numkey_info,
  om_text_print_keykey_entry,
  om_text_print_numkey_entry,
  om_text_print_numkeybar_entry,
  om_text_print_numkeycomparativebar_entry,
  om_text_print_bidimentional_map,
  om_text_print_hline,
  om_text_print_report_link,
};
void vi_print_statistics(struct vih *vih) {
  time_t elapsed = vih->endt - vih->startt;

  if (elapsed == 0) elapsed++;
  fprintf(stderr, "--\n%d lines processed in %ld seconds\n"
      "%d invalid lines, %d blacklisted referers\n",
      vih->processed, (long) elapsed,
      vih->invalid, vih->blacklisted);
}
int vi_postprocess_pageviews(struct vih *vih) {
  void **table;
  int len = ht_used(&vih->pageviews), i;

  if ((table = ht_get_array(&vih->pageviews)) == NULL) {
    fprintf(stderr, "Out of memory in vi_postprocess_pageviews()\n");
    return 1;
  }
  /* Run the hashtable in order to populate 'pageviews_grouped' */
  for (i = 0; i < len; i++) {
    int pv = (long) table[(i*2)+1]; /* pageviews of visit */
    int res;
    char *key;

    if (pv == 1) key = "1";
    else if (pv == 2) key = "2";
    else if (pv == 3) key = "3";
    else if (pv == 4) key = "4";
    else if (pv == 5) key = "5";
    else if (pv == 6) key = "6";
    else if (pv == 7) key = "7";
    else if (pv == 8) key = "8";
    else if (pv == 9) key = "9";
    else if (pv == 10) key = "10";
    else if (pv >= 11 && pv <= 20) key = "11-20";
    else if (pv >= 21 && pv <= 30) key = "21-30";
    else key = "> 30";

    res = vi_counter_incr(&vih->pageviews_grouped, key);
    if (res == 0) {
      free(table);
      return 1; /* out of memory */
    }
  }
  free(table);
  return 0;
}
int vi_postprocess(struct vih *vih) {
  if (vi_postprocess_pageviews(vih)) goto oom;
  return 0;
oom:
  vi_set_error(vih, "Out of memory");
  return 1;
}
void vi_print_header(FILE *fp) {
  Output->print_header(fp);
}
void vi_print_hline(FILE *fp) {
  Output->print_hline(fp);
}
void vi_print_information_report(FILE *fp, struct vih *vih) {
  char buf[VI_LINE_MAX];
  time_t now = time(NULL);
  snprintf(buf, VI_LINE_MAX, "Generated: %s", ctime(&now));
  Output->print_title(fp, "General information");
  Output->print_subtitle(fp, "Information about analyzed log files");
  Output->print_subtitle(fp, buf);
  Output->print_numkey_info(fp, "Number of entries processed", vih->processed);
  Output->print_numkey_info(fp, "Number of invalid entries", vih->invalid);
  Output->print_numkey_info(fp, "Processing time in seconds", (vih->endt)-(vih->startt));
}
void vi_print_report_links(FILE *fp) {
  void *l[] = {
    "Unique visitors in each day", NULL,
    "Unique visitors in each month", &Config_process_monthly_visitors,
    "Unique visitors from Google in each day", NULL,
    "Unique visitors from Google in each month", &Config_process_monthly_visitors,
    "Pageviews per visit", &Config_process_pageviews,
    "Weekday-Hour combined map", &Config_process_weekdayhour_map,
    "Month-Day combined map", &Config_process_monthday_map,
    "Requested pages", NULL,
    "Requested images and CSS", NULL,
    "Referers", NULL,
    "Referers by first time", &Config_process_referers_age,
    "Robots and web spiders", &Config_process_robots,
    "User agents", &Config_process_agents,
    "Operating Systems", &Config_process_os,
    "Browsers", &Config_process_browsers,
    "404 Errors", &Config_process_error404,
    "Domains", &Config_process_tld,
    "Googled pages", &Config_process_google,
    "Adsensed pages", &Config_process_google,
    "Google Keyphrases", &Config_process_google_keyphrases,
    "Google Keyphrases by first time", &Config_process_google_keyphrases_age,
    "Google Human Language", &Config_process_google_human_language,
    "Screen resolution", &Config_process_screen_info,
    "Screen color depth", &Config_process_screen_info,
    "Web trails", &Config_process_web_trails,
    "Weekday distribution", NULL,
    "Hours distribution", NULL,
  };
  unsigned int i, num = 0;

  Output->print_title(fp, "Generated reports");
  Output->print_subtitle(fp, "Click on the report name you want to see");
  for (i = 0; i < sizeof(l)/sizeof(void*); i += 2) {
    int active = l[i+1] == NULL ? 1 : *((int*)l[i+1]);
    if (active) num++;
  }
  Output->print_numkey_info(fp, "Number of reports generated", num);
  for (i = 0; i < sizeof(l)/sizeof(void*); i += 2) {
    int active = l[i+1] == NULL ? 1 : *((int*)l[i+1]);
    if (active)
      Output->print_report_link(fp, (char*)l[i]);
  }
}
int qsort_cmp_dates_generic(const void *a, const void *b, int off, int mul) {
  time_t ta, tb;
  void **A = (void**) a;
  void **B = (void**) b;
  char *dateA = (char*) *(A+off);
  char *dateB = (char*) *(B+off);

  ta = parse_date(dateA, NULL);
  tb = parse_date(dateB, NULL);
  if (ta == (time_t)-1 && tb == (time_t)-1) return 0;
  if (ta == (time_t)-1) return 1*mul;
  if (tb == (time_t)-1) return -1*mul;
  if (ta > tb) return 1*mul;
  if (ta < tb) return -1*mul;
  return 0;
}
int qsort_cmp_dates_key(const void *a, const void *b) {
  return qsort_cmp_dates_generic(a, b, 0, 1);
}
int qsort_cmp_months_key(const void *a, const void *b) {
  int ret;
  char dateA[VI_DATE_MAX];
  char dateB[VI_DATE_MAX];
  void *savedA, *savedB; /* backups of the original pointers */
  void **A = (void**) a;
  void **B = (void**) b;

  /* We use an hack here, in order to call qsort_cmp_dates_generic
   * even in this case, we substitute the hashtable entries
   * with versions of the strings prefixed with "01", so they
   * will be parseble by parse_date().
   * In pratice for "May/2004" we instead put "01/May/2004" and so on. */
  savedA = *A;
  savedB = *B;
  dateA[0] = dateB[0] = '0';
  dateA[1] = dateB[1] = '1';
  dateA[2] = dateB[2] = '/';
  dateA[3] = dateB[3] = '\0';
  vi_strlcat(dateA, (char*)*A, VI_DATE_MAX);
  vi_strlcat(dateB, (char*)*B, VI_DATE_MAX);
  *A = dateA;
  *B = dateB;
  ret = qsort_cmp_dates_generic(a, b, 0, 1);
  /* Restore */
  *A = savedA;
  *B = savedB;
  return ret;
}
void vi_print_visits_report(FILE *fp, struct vih *vih) {
  int days = ht_used(&vih->date), i, tot = 0, max = 0;
  int months;
  void **table;

  Output->print_title(fp, "Unique visitors in each day");
  Output->print_subtitle(fp, "Multiple hits with the same IP, user agent and access day, are considered a single visit");
  Output->print_numkey_info(fp, "Number of unique visitors",
      ht_used(&vih->visitors));
  Output->print_numkey_info(fp, "Different days in logfile",
      ht_used(&vih->date));

  if ((table = ht_get_array(&vih->date)) == NULL) {
    fprintf(stderr, "Out Of Memory in print_visits_report()\n");
    return;
  }
  qsort(table, days, sizeof(void*)*2, qsort_cmp_dates_key);
  for (i = 0; i < days; i++) {
    long value = (long) table[(i*2)+1];
    if (value > max)
      max = value;
    tot += value;
  }
  for (i = 0; i < days; i++) {
    char *key = table[i*2];
    long value = (long) table[(i*2)+1];
    Output->print_numkeybar_entry(fp, key, max, tot, value);
  }
  free(table);
  Output->print_hline(fp);

  /* Montly */
  if (Config_process_monthly_visitors == 0) return;
  tot = max = 0;
  months = ht_used(&vih->month);
  Output->print_title(fp, "Unique visitors in each month");
  Output->print_subtitle(fp, "Multiple hits with the same IP, user agent and access day, are considered a single visit");
  Output->print_numkey_info(fp, "Number of unique visitors",
      ht_used(&vih->visitors));
  Output->print_numkey_info(fp, "Different months in logfile",
      ht_used(&vih->month));

  if ((table = ht_get_array(&vih->month)) == NULL) {
    fprintf(stderr, "Out Of Memory in print_visits_report()\n");
    return;
  }
  qsort(table, months, sizeof(void*)*2, qsort_cmp_months_key);
  for (i = 0; i < months; i++) {
    long value = (long) table[(i*2)+1];
    if (value > max)
      max = value;
    tot += value;
  }
  for (i = 0; i < months; i++) {
    char *key = table[i*2];
    long value = (long) table[(i*2)+1];
    Output->print_numkeybar_entry(fp, key, max, tot, value);
  }
  free(table);
}
int vi_counter_val(struct hashtable *ht, char *key) {
  unsigned int idx;
  int r;
  long val;

  r = ht_search(ht, key, &idx);
  if (r == HT_NOTFOUND) {
    return 0;
  } else {
    val = (long) ht_value(ht, idx);
    return val;
  }
}
void vi_print_googlevisits_report(FILE *fp, struct vih *vih) {
  int days = ht_used(&vih->date), i, months;
  void **table;

  Output->print_title(fp, "Unique visitors from Google in each day");
  Output->print_subtitle(fp, "The red part of the bar expresses the percentage of visits originated from Google");
  Output->print_numkey_info(fp, "Number of unique visitors",
      ht_used(&vih->visitors));
  Output->print_numkey_info(fp, "Number of unique visitors from google",
      ht_used(&vih->googlevisitors));
  Output->print_numkey_info(fp, "Different days in logfile",
      ht_used(&vih->date));

  if ((table = ht_get_array(&vih->date)) == NULL) {
    fprintf(stderr, "Out Of Memory in print_visits_report()\n");
    return;
  }
  qsort(table, days, sizeof(void*)*2, qsort_cmp_dates_key);
  for (i = 0; i < days; i++) {
    char *key = table[i*2];
    long value = (long) table[(i*2)+1];
    long googlevalue;

    googlevalue = vi_counter_val(&vih->googledate, key);
    Output->print_numkeycomparativebar_entry(fp, key, value, googlevalue);
  }
  free(table);
  Output->print_hline(fp);

  /* Montly */
  if (Config_process_monthly_visitors == 0) return;
  months = ht_used(&vih->month);
  Output->print_title(fp, "Unique visitors from Google in each month");
  Output->print_subtitle(fp, "The red part of the bar expresses the percentage of visits originated from Google");
  Output->print_numkey_info(fp, "Number of unique visitors",
      ht_used(&vih->visitors));
  Output->print_numkey_info(fp, "Number of unique visitors from google",
      ht_used(&vih->googlevisitors));
  Output->print_numkey_info(fp, "Different months in logfile",
      ht_used(&vih->month));

  if ((table = ht_get_array(&vih->month)) == NULL) {
    fprintf(stderr, "Out Of Memory in print_visits_report()\n");
    return;
  }
  qsort(table, months, sizeof(void*)*2, qsort_cmp_months_key);
  for (i = 0; i < months; i++) {
    char *key = table[i*2];
    long value = (long) table[(i*2)+1];
    long googlevalue;

    googlevalue = vi_counter_val(&vih->googlemonth, key);
    Output->print_numkeycomparativebar_entry(fp, key, value, googlevalue);
  }
  free(table);
}
void vi_print_weekdayhour_map_report(FILE *fp, struct vih *vih) {
  char *xlabel[24] = {
    "00", "01", "02", "03", "04", "05", "06", "07",
    "08", "09", "10", "11", "12", "13", "14", "15",
    "16", "17", "18", "19", "20", "21", "22", "23"};
  char **ylabel = vi_wdname;
  int j, minj = 0, maxj = 0;
  int *hw = (int*) vih->weekdayhour;
  char buf[VI_LINE_MAX];

  /* Check idexes of minimum and maximum in the array. */
  for (j = 0; j < 24*7; j++) {
    if (hw[j] > hw[maxj])
      maxj = j;
    if (hw[j] < hw[minj])
      minj = j;
  }

  Output->print_title(fp, "Weekday-Hour combined map");
  Output->print_subtitle(fp, "Brighter means higher level of hits");
  snprintf(buf, VI_LINE_MAX, "Hour with max traffic starting at %s %s:00 with hits",
      ylabel[maxj/24], xlabel[maxj%24]);
  Output->print_numkey_info(fp, buf, hw[maxj]);
  snprintf(buf, VI_LINE_MAX, "Hour with min traffic starting at %s %s:00 with hits",
      ylabel[minj/24], xlabel[minj%24]);
  Output->print_numkey_info(fp, buf, hw[minj]);
  Output->print_hline(fp);
  Output->print_bidimentional_map(fp, 24, 7, xlabel, ylabel, hw);
}
void vi_print_monthday_map_report(FILE *fp, struct vih *vih) {
  char *xlabel[31] = {
    "01", "02", "03", "04", "05", "06", "07", "08",
    "09", "10", "11", "12", "13", "14", "15", "16",
    "17", "18", "19", "20", "21", "22", "23", "24",
    "25", "26", "27", "28", "29", "30", "31"};
  char *ylabel[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
  };
  int j, minj = 0, maxj = 0;
  int *md = (int*) vih->monthday;
  char buf[VI_LINE_MAX];

  /* Check idexes of minimum and maximum in the array. */
  for (j = 0; j < 12*31; j++) {
    if (md[j] > md[maxj])
      maxj = j;
    if (md[j] != 0 && (md[j] < md[minj] || md[minj] == 0))
      minj = j;
  }

  Output->print_title(fp, "Month-Day combined map");
  Output->print_subtitle(fp, "Brighter means higher level of hits");
  snprintf(buf, VI_LINE_MAX, "Day with max traffic is %s %s with hits",
      ylabel[maxj/31], xlabel[maxj%31]);
  Output->print_numkey_info(fp, buf, md[maxj]);
  snprintf(buf, VI_LINE_MAX, "Day with min traffic is %s %s with hits",
      ylabel[minj/31], xlabel[minj%31]);
  Output->print_numkey_info(fp, buf, md[minj]);
  Output->print_hline(fp);
  Output->print_bidimentional_map(fp, 31, 12, xlabel, ylabel, md);
}
int qsort_cmp_long_value(const void *a, const void *b) {
  void **A = (void**) a;
  void **B = (void**) b;
  long la = (long) *(A+1);
  long lb = (long) *(B+1);
  if (la > lb) return -1;
  if (lb > la) return 1;
  return 0;
}
void vi_print_generic_keyvalbar_report(FILE *fp, char *title, char *subtitle, char *info, int maxlines, struct hashtable *ht, int(*compar)(const void *, const void *)) {
  int items = ht_used(ht), i, max = 0, tot = 0;
  void **table;

  Output->print_title(fp, title);
  Output->print_subtitle(fp, subtitle);
  Output->print_numkey_info(fp, info, items);
  if ((table = ht_get_array(ht)) == NULL) {
    fprintf(stderr, "Out of memory in print_generic_report()\n");
    return;
  }
  qsort(table, items, sizeof(void*)*2, compar);
  for (i = 0; i < items; i++) {
    long value = (long) table[(i*2)+1];
    tot += value;
    if (value > max) max = value;
  }
  for (i = 0; i < items; i++) {
    char *key = table[i*2];
    long value = (long) table[(i*2)+1];
    if (i >= maxlines) break;
    if (key[0] == '\0')
      Output->print_numkeybar_entry(fp, "none", max, tot, value);
    else
      Output->print_numkeybar_entry(fp, key, max, tot, value);
  }
  free(table);
}
void vi_print_generic_keyval_report(FILE *fp, char *title, char *subtitle, char *info, int maxlines, struct hashtable *ht, int(*compar)(const void *, const void *)) {
  int items = ht_used(ht), i;
  void **table;

  Output->print_title(fp, title);
  Output->print_subtitle(fp, subtitle);
  Output->print_numkey_info(fp, info, items);
  if ((table = ht_get_array(ht)) == NULL) {
    fprintf(stderr, "Out of memory in print_generic_report()\n");
    return;
  }
  qsort(table, items, sizeof(void*)*2, compar);
  for (i = 0; i < items; i++) {
    char *key = table[i*2];
    long value = (long) table[(i*2)+1];
    if (i >= maxlines) break;
    if (key[0] == '\0')
      Output->print_numkey_entry(fp, "none", value, NULL,
          i+1);
    else
      Output->print_numkey_entry(fp, key, value, NULL, i+1);
  }
  free(table);
}
void vi_print_pageviews_report(FILE *fp, struct vih *vih) {
  vi_print_generic_keyvalbar_report(
      fp,
      "Pageviews per visit",
      "Number of pages requested per visit",
      "Only documents are counted (not images). Reported ranges:",
      100,
      &vih->pageviews_grouped,
      qsort_cmp_long_value);
}
void vi_print_pages_report(FILE *fp, struct vih *vih) {
  vi_print_generic_keyval_report(
      fp,
      "Requested pages",
      "Page requests ordered by hits",
      "Different pages requested",
      Config_max_pages,
      &vih->pages,
      qsort_cmp_long_value);
}
void vi_print_images_report(FILE *fp, struct vih *vih) {
  vi_print_generic_keyval_report(
      fp,
      "Requested images and CSS",
      "Images and CSS requests ordered by hits",
      "Different images and CSS requested",
      Config_max_images,
      &vih->images,
      qsort_cmp_long_value);
}
void vi_print_referers_report(FILE *fp, struct vih *vih) {
  vi_print_generic_keyval_report(
      fp,
      "Referers",
      "Referers ordered by visits (google excluded)",
      "Different referers",
      Config_max_referers,
      &vih->referers,
      qsort_cmp_long_value);
}
void vi_print_generic_keytime_report(FILE *fp, char *title, char *subtitle, char *info, int maxlines, struct hashtable *ht, int(*compar)(const void *, const void *)) {
  int items = ht_used(ht), i;
  void **table;

  Output->print_title(fp, title);
  Output->print_subtitle(fp, subtitle);
  Output->print_numkey_info(fp, info, items);
  if ((table = ht_get_array(ht)) == NULL) {
    fprintf(stderr, "Out Of Memory in print_generic_keytime_report()\n");
    return;
  }
  qsort(table, items, sizeof(void*)*2, compar);
  for (i = 0; i < items; i++) {
    struct tm *tm;
    char ftime[1024];
    char *url = table[i*2];
    time_t time = (time_t) table[(i*2)+1];
    if (i >= maxlines) break;
    tm = localtime(&time);
    if (tm) {
      ftime[0] = '\0';
      strftime(ftime, 1024, "%d/%b/%Y", tm);
      Output->print_keykey_entry(fp, ftime,
          (url[0] == '\0') ? "none" : url, i+1);
    }
  }
  free(table);
}
int qsort_cmp_time_value(const void *a, const void *b) {
  void **A = (void**) a;
  void **B = (void**) b;
  time_t ta = (time_t) *(A+1);
  time_t tb = (time_t) *(B+1);
  if (ta > tb) return -1;
  if (tb > ta) return 1;
  return 0;
}
void vi_print_referers_age_report(FILE *fp, struct vih *vih) {
  vi_print_generic_keytime_report(
      fp,
      "Referers by first time",
      "Referers ordered by first time date, newer on top (referers from google excluded)",
      "Different referers",
      Config_max_referers_age,
      &vih->referersage,
      qsort_cmp_time_value);
}
void vi_print_robots_report(FILE *fp, struct vih *vih) {
  vi_print_generic_keyval_report(
      fp,
      "Robots and web spiders",
      "Agents requesting robots.txt. MSIECrawler excluded.",
      "Total number of different robots",
      Config_max_robots,
      &vih->robots,
      qsort_cmp_long_value);
}
void vi_print_agents_report(FILE *fp, struct vih *vih) {
  vi_print_generic_keyval_report(
      fp,
      "User agents",
      "The entire user agent string ordered by visits",
      "Different agents",
      Config_max_agents,
      &vih->agents,
      qsort_cmp_long_value);
}
void vi_print_os_report(FILE *fp, struct vih *vih) {
  vi_print_generic_keyvalbar_report(
      fp,
      "Operating Systems",
      "Operating Systems by visits",
      "Different operating systems listed",
      100,
      &vih->os,
      qsort_cmp_long_value);
}
void vi_print_browsers_report(FILE *fp, struct vih *vih) {
  vi_print_generic_keyvalbar_report(
      fp,
      "Browsers",
      "Browsers used by visits",
      "Different browsers listed",
      100,
      &vih->browsers,
      qsort_cmp_long_value);
}
void vi_print_error404_report(FILE *fp, struct vih *vih) {
  vi_print_generic_keyval_report(
      fp,
      "404 Errors",
      "Requests for missing documents",
      "Different missing documents requested",
      Config_max_error404,
      &vih->error404,
      qsort_cmp_long_value);
}
void vi_print_tld_report(FILE *fp, struct vih *vih) {
  vi_print_generic_keyvalbar_report(
      fp,
      "Domains",
      "Top Level Domains sorted by visits",
      "Total number of Top Level Domains",
      Config_max_tld,
      &vih->tld,
      qsort_cmp_long_value);
}
void vi_print_googled_report(FILE *fp, struct vih *vih) {
  vi_print_generic_keytime_report(
      fp,
      "Googled pages",
      "Pages accessed by the Google crawler, last access reported",
      "Number of pages googled",
      Config_max_googled,
      &vih->googled,
      qsort_cmp_time_value);
}
void vi_print_adsensed_report(FILE *fp, struct vih *vih) {
  vi_print_generic_keytime_report(
      fp,
      "Adsensed pages",
      "Pages accessed by the Adsense crawler, last access reported",
      "Number of pages adsensed",
      Config_max_adsensed,
      &vih->adsensed,
      qsort_cmp_time_value);
}
void vi_urlencode(char *d, char *s, int n) {
  if (n < 1) return;
  n--;
  while(*s && n > 0) {
    int c = *s;
    if ((c >= 'A' && c <= 'Z') ||
        (c >= 'a' && c <= 'z') ||
        (c >= '0' && c <= '9'))
    {
      *d++ = c;
      n--;
    } else if (c == ' ') {
      *d++ = '+';
      n--;
    } else if (c == '\n') {
      if (n < 6) break;
      memcpy(d, "%0d%0a", 6);
      d += 6;
      n -= 6;
    } else {
      unsigned int t;
      char *hexset = "0123456789abcdef";

      if (n < 3) break;
      t = (unsigned) c;
      *d++ = '%';
      *d++ = hexset [(t & 0xF0) >> 4];
      *d++ = hexset [(t & 0x0F)];
      n -= 3;
    }
    s++;
  }
  *d = '\0';
}
void vi_print_keyphrases_report(FILE *fp, char *title, char *subtitle, char *info, int maxlines, struct hashtable *ht, int(*compar)(const void *, const void *)) {
  int items = ht_used(ht), i;
  void **table;

  Output->print_title(fp, title);
  Output->print_subtitle(fp, subtitle);
  Output->print_numkey_info(fp, info, items);
  if ((table = ht_get_array(ht)) == NULL) {
    fprintf(stderr, "Out of memory in print_keyphrases_report()\n");
    return;
  }
  qsort(table, items, sizeof(void*)*2, compar);
  for (i = 0; i < items; i++) {
    char *key = table[i*2];
    long value = (long) table[(i*2)+1];
    if (i >= maxlines) break;
    if (key[0] == '\0')
      Output->print_numkey_entry(fp, "none", value, NULL,
          i+1);
    else {
      char *p;
      char link[VI_LINE_MAX];
      char aux[VI_LINE_MAX];
      char encodedkey[VI_LINE_MAX];

      vi_strlcpy(link, "http://www.google.com/search?q=", VI_LINE_MAX);
      vi_strlcpy(aux, key, VI_LINE_MAX);
      p = strrchr(aux, '(');
      if (p) {
        if (p > aux) p--; /* seek the space on left */
        *p = '\0';
      }
      vi_urlencode(encodedkey, aux, VI_LINE_MAX);
      vi_strlcat(link, encodedkey, VI_LINE_MAX);
      Output->print_numkey_entry(fp, key, value, link, i+1);
    }
  }
  free(table);
}
void vi_print_google_keyphrases_report(FILE *fp, struct vih *vih) {
  vi_print_keyphrases_report(
      fp,
      "Google Keyphrases",
      "Keyphrases used in google searches ordered by visits",
      "Total number of keyphrases",
      Config_max_google_keyphrases,
      &vih->googlekeyphrases,
      qsort_cmp_long_value);
}
void vi_print_google_keyphrases_age_report(FILE *fp, struct vih *vih) {
  vi_print_generic_keytime_report(
      fp,
      "Google Keyphrases by first time",
      "Keyphrases ordered by first time date, newer on top",
      "Different referers",
      Config_max_google_keyphrases_age,
      &vih->googlekeyphrasesage,
      qsort_cmp_time_value);
}
void vi_print_google_human_language_report(FILE *fp, struct vih *vih) {
  vi_print_generic_keyval_report(
      fp,
      "Google Human Language",
      "The 'hl' field in the query string of google searches",
      "Different human languages",
      1000,
      &vih->googlehumanlanguage,
      qsort_cmp_long_value);
}
void vi_print_screen_res_report(FILE *fp, struct vih *vih) {
  vi_print_generic_keyval_report(
      fp,
      "Screen resolution",
      "user screen width x height resolution",
      "Different resolutions",
      1000,
      &vih->screenres,
      qsort_cmp_long_value);
}
void vi_print_screen_depth_report(FILE *fp, struct vih *vih) {
  vi_print_generic_keyval_report(
      fp,
      "Screen color depth",
      "user screen color depth in bits per pixel",
      "Different color depths",
      1000,
      &vih->screendepth,
      qsort_cmp_long_value);
}
void vi_print_trails_report(FILE *fp, struct vih *vih) {
  vi_print_generic_keyval_report(
      fp,
      "Web trails",
      "Referer -> Target common moves",
      "Total number of trails",
      Config_max_trails,
      &vih->trails,
      qsort_cmp_long_value);
}
void vi_print_weekdays_report(FILE *fp, struct vih *vih) {
  int i, max = 0, tot = 0;
  for (i = 0; i < 7; i++) {
    if (vih->weekday[i] > max)
      max = vih->weekday[i];
    tot += vih->weekday[i];
  }
  Output->print_title(fp, "Weekdays distribution");
  Output->print_subtitle(fp, "Percentage of hits in every day of the week");
  for (i = 0; i < 7; i++) {
    Output->print_numkeybar_entry(fp, vi_wdname[i], max, tot, vih->weekday[i]);
  }
}
void vi_print_hours_report(FILE *fp, struct vih *vih) {
  int i, max = 0, tot = 0;
  for (i = 0; i < 24; i++) {
    if (vih->hour[i] > max)
      max = vih->hour[i];
    tot += vih->hour[i];
  }
  Output->print_title(fp, "Hours distribution");
  Output->print_subtitle(fp, "Percentage of hits in every hour of the day");
  for (i = 0; i < 24; i++) {
    char buf[8];
    sprintf(buf, "%02d", i);
    Output->print_numkeybar_entry(fp, buf, max, tot, vih->hour[i]);
  }
}
void vi_print_footer(FILE *fp) {
  Output->print_footer(fp);
}
int vi_print_report(char *of, struct vih *vih) {
  FILE *fp;

  if (of == NULL) {
    fp = stdout;
  } else {
    fp = fopen(of, "w");
    if (fp == NULL) {
      vi_set_error(vih, "Writing the report to '%s': %s",
          of, strerror(errno));
      return 1;
    }
  }

  /* Disable specific reports when there is no data. */
  if (ht_used(&vih->screenres) == 0)
    Config_process_screen_info = 0;
  /* Do some data postprocessing needed to generate reports */
  if (vi_postprocess(vih))
    return 1;
  /* Report generation */
  vi_print_header(fp);
  vi_print_hline(fp);
  vi_print_information_report(fp, vih);
  vi_print_hline(fp);
  vi_print_report_links(fp);
  vi_print_hline(fp);
  vi_print_visits_report(fp, vih);
  vi_print_hline(fp);
  vi_print_googlevisits_report(fp, vih);
  vi_print_hline(fp);
  if (Config_process_weekdayhour_map) {
    vi_print_weekdayhour_map_report(fp, vih);
    vi_print_hline(fp);
  }
  if (Config_process_monthday_map) {
    vi_print_monthday_map_report(fp, vih);
    vi_print_hline(fp);
  }
  if (Config_process_pageviews) {
    vi_print_pageviews_report(fp, vih);
    vi_print_hline(fp);
  }
  vi_print_pages_report(fp, vih);
  vi_print_hline(fp);
  vi_print_images_report(fp, vih);
  vi_print_hline(fp);
  vi_print_referers_report(fp, vih);
  vi_print_hline(fp);
  if (Config_process_referers_age) {
    vi_print_referers_age_report(fp, vih);
    vi_print_hline(fp);
  }
  if (Config_process_robots) {
    vi_print_robots_report(fp, vih);
    vi_print_hline(fp);
  }
  if (Config_process_agents) {
    vi_print_agents_report(fp, vih);
    vi_print_hline(fp);
  }
  if (Config_process_os) {
    vi_print_os_report(fp, vih);
    vi_print_hline(fp);
  }
  if (Config_process_browsers) {
    vi_print_browsers_report(fp, vih);
    vi_print_hline(fp);
  }
  if (Config_process_error404) {
    vi_print_error404_report(fp, vih);
    vi_print_hline(fp);
  }
  if (Config_process_tld) {
    vi_print_tld_report(fp, vih);
    vi_print_hline(fp);
  }
  if (Config_process_google) {
    vi_print_googled_report(fp, vih);
    vi_print_hline(fp);
    vi_print_adsensed_report(fp, vih);
    vi_print_hline(fp);
  }
  if (Config_process_google_keyphrases) {
    vi_print_google_keyphrases_report(fp, vih);
    vi_print_hline(fp);
  }
  if (Config_process_google_keyphrases) {
    vi_print_google_keyphrases_age_report(fp, vih);
    vi_print_hline(fp);
  }
  if (Config_process_google_human_language) {
    vi_print_google_human_language_report(fp, vih);
    vi_print_hline(fp);
  }
  if (Config_process_screen_info) {
    vi_print_screen_res_report(fp, vih);
    vi_print_hline(fp);
    vi_print_screen_depth_report(fp, vih);
    vi_print_hline(fp);
  }
  if (Config_process_web_trails) {
    vi_print_trails_report(fp, vih);
    vi_print_hline(fp);
  }
  vi_print_weekdays_report(fp, vih);
  vi_print_hline(fp);
  vi_print_hours_report(fp, vih);
  vi_print_hline(fp);
  vi_print_hline(fp);
  vi_print_footer(fp);
  if (of != NULL)
    fclose(fp);
  return 0;
}
void vi_print_graphviz(struct vih *vih) {
  int items = ht_used(&vih->trails), i, max = 0, tot = 0;
  void **table;

  printf("digraph webtrails {\n");
  printf("\tgraph [splines=true overlap=false rankdir=LR]\n");
  printf("\tnode [color=lightblue2,style=\"filled\"]\n");
  printf("\tedge [style=bold]\n");
  if ((table = ht_get_array(&vih->trails)) == NULL) {
    fprintf(stderr, "Out of memory in vi_print_graphviz()\n");
    return;
  }
  qsort(table, items, sizeof(void*)*2, qsort_cmp_long_value);
  for (i = 0; i < items; i++) {
    long value = (long) table[(i*2)+1];
    tot += value;
    if (i > Config_max_trails) continue;
    if (max < value)
      max = value;
  }
  if (max == 0) max = 1; /* avoid division by zero */
  if (tot == 0) tot = 1;
  for (i = 0; i < items; i++) {
    int color;
    char *key = table[i*2];
    char *t;
    long value = (long) table[(i*2)+1];
    float percentage = ((float)value/tot)*100;
    if (i > Config_max_trails) break;
    color = (value*255)/max;
    t = strstr(key, " -> ");
    *t = '\0'; /* alter */
    printf("\t\"%s\" -> \"%s\" [color=\"#%02X00%02X\" label=\"%.2f\"]\n", key, t+4, color, 255-color, percentage);
    *t = ' '; /* restore */
  }
  if (!Config_graphviz_ignorenode_google)
    printf("\tGoogle [color=\"#c0ffc0\"]\n");
  if (!Config_graphviz_ignorenode_external)
    printf("\t\"External Link\" [color=\"#c0ffc0\"]\n");
  if (!Config_graphviz_ignorenode_noreferer)
    printf("\t\"No Referer\" [color=\"#c0ffc0\"]\n");
  free(table);
  printf("}\n");
}
int main(int argc, char **argv) {
  struct vih *vih;
  char *filename;
  int minargc=2;
  if( argc < minargc ) {
    printf("Insufficient args %d<%d\n",argc, minargc);
    exit(-1);
  }
  filename = argv[1];
  printf("%s\n", filename);
  setlocale(LC_ALL, "C");
  vih = vi_new();
  Config_process_agents = 1;
  Config_process_google = 1;
  Config_process_google_keyphrases = 1;
  Config_process_google_keyphrases_age = 1;
  Config_process_google_human_language = 1;
  Config_process_weekdayhour_map = 1;
  Config_process_monthday_map = 1;
  Config_process_referers_age = 1;
  Config_process_tld = 1;
  Config_process_os = 1;
  Config_process_browsers = 1;
  Config_process_error404 = 1;
  Config_process_pageviews = 1;
  Config_process_robots = 1;
  Config_process_screen_info = 1;
  //graphviz opts
  Config_graphviz_mode = 1;
  Config_graphviz_ignorenode_google = 0;
  Config_graphviz_ignorenode_external = 0;
  Config_graphviz_ignorenode_noreferer = 0;

  Output = &OutputModuleText;
  if (vi_scan(vih, filename)) {
    fprintf(stderr, "%s: %s\n", filename, vi_get_error(vih));
    exit(1);
  }
  vi_print_graphviz(vih);
  vi_print_report(NULL,vih);
  vi_print_statistics(vih);
  vi_free(vih);
  return 0;
}
