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
#if 0
static int vi_monthdays[12] = {31, 29, 31, 30, 31, 30 , 31, 31, 30, 31, 30, 31};
static char *vi_wdname[7] = {"Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"};
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
void print_logline( struct logline * ll ) {
  printf ("%s %s %s %s %s %s %s\n", ll->host, ll->date, ll->hour, ll->timezone, ll->req, ll->ref, ll->agent);
}
int vi_process_line(struct vih *vih, char *l) {
  struct logline ll;
  char origline[VI_LINE_MAX];
  if (Config_grep_pattern_num) {
    if (vi_match_line(l) == 0)
      return 0; /* No match? skip. */
  }
  vih->processed++;
  if (vi_parse_line(&ll, l) == 0) {
    print_logline( &ll );
    return 0;
  } else {
    vih->invalid++;
    if (Config_debug) {
      fprintf(stderr, "Invalid line: %s\n", origline);
    }
    return 0;
  }
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
  Config_graphviz_mode = 1;
  Config_graphviz_ignorenode_google = 0;
  Config_graphviz_ignorenode_external = 0;
  Config_graphviz_ignorenode_noreferer = 1;
  if (vi_scan(vih, filename)) {
    fprintf(stderr, "%s: %s\n", filename, vi_get_error(vih));
    exit(1);
  }
  vi_free(vih);
  return 0;
}
