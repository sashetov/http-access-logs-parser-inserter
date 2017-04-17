//basic "node" container used with htab
typedef struct node {
  char  *name;
  size_t nval;
} node;
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
static hashval_t ht_get_node_hash(const void *n);
node   *node_init (const char *s);
void   ht_init (htab_t *table, int size);
static int nodes_equal(const void *entry, const void *element);
static void node_delete(node *n);
static void key_del(void *key);
size_t ht_kget_nval(htab_t table, const char *str);
static void ht_kadd_val_to_k_nval(htab_t table, const char *str, size_t nval);
httpaccess_metrics *h_metrics_init(void);
void h_metrics_set_error(httpaccess_metrics *h_metrics, char *fmt, ...);
void h_metrics_reset_hashtables(httpaccess_metrics *h_metrics);
void h_metrics_set_error(httpaccess_metrics *h_metrics, char *fmt, ...);
void h_metrics_clear_error(httpaccess_metrics *h_metrics);
void h_metrics_free(httpaccess_metrics *h_metrics);

