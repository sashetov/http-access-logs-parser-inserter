#include "ht_nodes.h"

static hashval_t ht_get_node_hash(const void *n) {
  return htab_hash_string(((node *) n)->name);
}
node *node_init(const char *s) {
  node *n = malloc(sizeof(node));
  n->name= (char *) malloc(strlen(s) + 1);
  strcpy(n->name, s);
  return n;
}
void ht_init(htab_t *table, int size) {
  if( size < HT_ALLOC_SIZE_MAX ) {
    table = htab_create_alloc(size, ht_get_node_hash, nodes_equal, key_del, calloc, free);
  }
  else{
    table = htab_create_alloc(HT_ALLOC_SIZE_MAX, ht_get_node_hash, nodes_equal, key_del, calloc, free);
  }
}
static int nodes_equal(const void *entry, const void *element) {
  return strcmp(((const node *) entry)->name, ((const node *) element)->name) == 0;
}
static void node_delete(node *n) {
  free(n->name);
  free(n);
}
static void key_del(void *key) {
  node_delete((node *) key);
}
size_t ht_kget_nval(htab_t table, const char *str) {
  node *k= node_init(str);
  node *n= (node *) htab_find(table, k);
  if (n == NULL) {
    return -1;
  }
  return n->nval;
}
static void ht_kadd_val_to_k_nval(htab_t table, const char *str, size_t nval) {
  signed long nval_curr= ht_kget_nval(table,str);
  if( nval_curr > 0 ){
    nval += nval_curr;
  }
  node *node = node_init(str);
  void **slot = htab_find_slot(table, node, INSERT);
  if (slot != NULL) {
    node->nval= nval;
    *slot = node;
  }
}
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
