#ifndef ___HT_NODE__
#include "ht_nodes.h"
#endif

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
