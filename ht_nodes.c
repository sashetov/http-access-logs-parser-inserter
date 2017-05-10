#ifndef __HT_NODE__
#include <stdlib.h>
#include "ht_nodes.h"

#ifndef HL_HASHTABLE_H
#include "hashtable.h"
#endif

static hashval_t ht_get_node_hash(const void *n) {
  return htab_hash_string(((node *) n)->name);
}
node *node_init(const char *s, int val ) {
  node *n = malloc(sizeof(node));
  n->name= (char *) malloc(strlen(s) + 1);
  strcpy(n->name, s);
  n->nval = val;
  return n;
}
/*void node_ht_init( htab_t * table, size_t size ) {
  if( size < HT_ALLOC_SIZE_MAX ) {
    table = htab_create_alloc(size,              ht_get_node_hash, nodes_equal, key_del, calloc, free);
  }
  else{
    table = htab_create_alloc(HT_ALLOC_SIZE_MAX, ht_get_node_hash, nodes_equal, key_del, calloc, free);
  }
}
void node_ht_init( hashtable_t * table, size_t size ) {
  ht_create( size, 0, NULL );
}
int nodes_equal(const void *entry, const void *element) {
  return strcmp(((const node *) entry)->name, ((const node *) element)->name) == 0;
}
static void node_delete(node *n) {
  free(n->name);
  free(n);
}
static void key_del(void *key) {
  node_delete((node *) key);
}
size_t ht_kget_nval( htab_t table, const char *str) {
  node *k= node_init(str);
  node *n= (node *) htab_find(table, k);
  if (n == NULL) {
    return -1;
  }
  return n->nval;
}*/
size_t ht_kget_nval( hashtable_t *table, const char *str ){
  node *n = (node *) ht_get( table, str, strlen(str), sizeof( node ) );
  if (n == NULL) {
    return -1;
  }
  int nval = n->nval;
  free(n);
  return n->nval;
}

void ht_kadd_val_to_k_nval(hashtable_t *table, const char *str, int val) {
  if( ! ht_exists(table, str, strlen(str))){
    node * data = (node *)node_init(str,val);
    ht_set( table, str, strlen(str), data, sizeof(node) );
    free( data );
  }
  else {
    node * n = (node *) ht_get(table, str, strlen(str), sizeof(node));
    n->nval += val;
    free(n);
  }
}
void ht_insert_next_val( hashtable_t *table, const char *key) {

}
#endif
