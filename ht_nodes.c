#ifndef __HT_NODE__
#include <stdlib.h>
#include "ht_nodes.h"
#ifndef HL_HASHTABLE_H
#include "hashtable.h"
#endif
node *node_init(const char *s, int val ) {
  node *n = (node *) malloc(sizeof(node));
  n->name= (char *) malloc(strlen(s) + 1);
  strcpy(n->name, s);
  n->nval = val;
  return n;
}
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
#endif
