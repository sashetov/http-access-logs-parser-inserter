#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "hashtab.h"
typedef struct node {
  char *str;
  size_t count;
} node;
node *node_init(const char *s) {
  node *node = malloc(sizeof(node));
  node->str = (char *) malloc(strlen(s) + 1);
  strcpy(node->str, s);
  return node;
}
static void node_delete(node *node) {
  free(node->str);
  free(node);
}
static int nodes_equal(const void *entry, const void *element) {
  return strcmp(((const node *) entry)->str, ((const node *) element)->str) == 0;
}
static void key_del(void *key) {
  node_delete((node *) key);
}
static hashval_t hash(const void *n) {
  return htab_hash_string(((node *) n)->str);
}
size_t get(htab_t table, const char *str) {
  node *k= node_init(str);
  node *n= (node *) htab_find(table, k);
  if (n == NULL) {
    return -1;
  }
  return n->count;
}
static void add(htab_t table, const char *str, size_t count) {
  signed long currcount = get(table,str);
  if( currcount > 0 ){
    count += currcount;
  }
  node *node = node_init(str);
  void **slot = htab_find_slot(table, node, INSERT);
  if (slot != NULL) {
    node->count = count;
    *slot = node;
  }
}
int main(int argc, const char *argv[]) {
  htab_t table = htab_create_alloc(1000, hash, nodes_equal, key_del, calloc, free);
  long i;
  for( i=0; i< 100000; i++ ) {
    add(table, "world", 1);
  }
  printf("bar: %lu\n", get(table, "world"));
  return 0;
}
