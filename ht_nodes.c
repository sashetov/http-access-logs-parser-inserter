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
name_version_node_t * name_version_node_init(char *name, char *version, int val ) {
  name_version_node_t *n = (name_version_node_t *) malloc(sizeof(name_version_node_t));
  n->name= strdup(name);
  n->version= strdup(version);
  n->nval = (size_t) val;
  return n;
}
void free_node( node  * n) {
  free(n->name);
  free(n);
}
void free_name_version_node( name_version_node_t * n) {
  free(n->name);
  free(n->version);
  free(n);
}
size_t ht_kget_nval( hashtable_t *table, const char *key ){
  node *n = (node *) ht_get_copy( table, (void * )key , strlen(key)+1, (size_t *)sizeof( node ));
  if (n == NULL) {
    return -1;
  }
  int nval = n->nval;
  free_node(n);
  return (size_t) nval;
}
void ht_kadd_val_to_k_nval(hashtable_t *table, const char *str, int val) {
  if( ! ht_exists(table, str, strlen(str))){
    node * data = (node *)node_init(str,val);
    ht_set_copy( table, str, strlen(str)+1, data, sizeof(node),NULL,NULL );
    free_node( data );
  }
  else {
    node * n = (node *) ht_get_copy(table, str, strlen(str)+1, sizeof(node));
    n->nval += val;
    free_node(n);
  }
}
void ht_add_nvers_to_k_nval(hashtable_t *table, char *key, char *name, char *vers, int val) {
  if( ! ht_exists(table, key, strlen(key)+1)){
    name_version_node_t * n= name_version_node_init(name,vers,val);
    ht_set_copy( table, key, strlen(key)+1, n, sizeof(name_version_node_t),NULL,NULL);
    free_name_version_node( n );
  }
  else {
    name_version_node_t * n = (name_version_node_t *) ht_get_copy( table, key, strlen(key)+1, sizeof(name_version_node_t));
    n->nval += val;
    ht_set_copy( table, key, strlen(key)+1, n, sizeof(name_version_node_t),NULL,NULL);
    free_name_version_node( n );
  }
}
#endif
