#ifndef __HT_NODE__
#include <stdlib.h>
#include "ht_nodes.h"
#ifndef HL_HASHTABLE_H
#include "hashtable.h"
#endif
node *node_init(char *s, int val ) {
  node *n = (node *) malloc(sizeof(node));
  n->name = (char *) malloc(strlen(s)+1 );
  memcpy(n->name,s,strlen(s)+1);
  n->name[strlen(n->name)] = '\0';
  n->nval = val;
  return n;
}
name_version_node_t * name_version_node_init(char *name, char *version, int val ) {
  name_version_node_t *n = (name_version_node_t *) malloc(sizeof(name_version_node_t));
  n->name=strdup( name);
  n->version=strdup( version);
  n->nval = (size_t) val;
  return n;
}
void free_node( node  * n, char * which ) {
  free(n->name);
  free(n);
}
void free_node_cb( node  * n ) {
  free(n->name);
  free(n);
}
void free_name_version_node( name_version_node_t * n, char * which ) {
  //printf("which %s n->name %s addr %p n->version %s addr %p &n %p\n",which ,n->name, &n->name, n->version, &n->version, &n);
  free(n->name);
  free(n->version);
  free(n);
}
size_t ht_kget_nval( hashtable_t *table, const char *key, char * which ){
  size_t * size = ( size_t * ) sizeof(node);
  node *n = (node *) ht_get( table, (void * )key , strlen(key)+1, size);
  if (n == NULL) {
    return -1;
  }
  int nval = n->nval;
  return (size_t) nval;
}
void ht_add_nvers_to_k_nval( hashtable_t *table, char *key, char *name, char *vers, int val, char * which ) {
  name_version_node_t * n;
  size_t * size = ( size_t * ) sizeof(name_version_node_t);
  if( ! ht_exists(table, key, strlen(key)+1)){
    n = name_version_node_init(name,vers,val);
    ht_set( table, key, strlen(key)+1, n, size);
    free_name_version_node(n, which);
  }
  else {
    n = (name_version_node_t *) ht_get( table, key, strlen(key)+1, size);
    n->nval += val;
    ht_set( table, key, strlen(key)+1, n, size);
  }
}
#endif
