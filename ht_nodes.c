#ifndef __HT_NODE__
#include <stdlib.h>
#include "ht_nodes.h"
#ifndef HL_HASHTABLE_H
#include "hashtable.h"
#endif
node *node_init(const char *s, int val ) {
  node *n = (node *) malloc(sizeof(node));
  if(strlen(s)){
    n->name= strdup(s);
    //printf("allocating '%s'(%d), '%s'(%d)\n",s,strlen(s),n->name,strlen(n->name));
  }
  n->nval = val;
  return n;
}

void free_node( node  * n) {
  if(strlen(n->name)){
    //printf("freeing '%s', size: %d\n",n->name, strlen(n->name));
    free(n->name);
  }
  free(n);
}

size_t ht_kget_nval( hashtable_t *table, const char *str ){
  node *n = (node *) ht_get_copy(
      table, (void * )str, strlen(str)+1, (size_t *)sizeof( node ));

  if (n == NULL) {
    return -1;
  }
  int nval = n->nval;
  free(n);
  return (size_t) nval;
}
void ht_kadd_val_to_k_nval(hashtable_t *table, const char *str, int val) {
  if( ! ht_exists(table, str, strlen(str))){
    node * data = (node *)node_init(str,val);
    ht_set_copy( table, str, strlen(str)+1, data, sizeof(node),NULL,NULL );
    free_node( data );
  }
  else {
    node * n = (node *) ht_get(table, str, strlen(str), sizeof(node));
    n->nval += val;
    free_node(n);
  }
}
#endif
