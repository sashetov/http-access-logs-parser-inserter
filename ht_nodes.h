#ifdef __cplusplus
extern "C" {
#endif
#define ERROR_MAX               100
#define HT_ALLOC_SIZE_MAX       404800
#define HT_ALLOC_SIZE_DEFAULT   10240
#include <stdlib.h>
#include <stdint.h>
#ifndef __HT_NODE__
typedef struct node {
  size_t nval;
  char  name[1000];
} node;
typedef struct name_version_node {
  size_t nval;
  char  name[1000];
  char  version[1000];
} name_version_node_t;
#define __HT_NODE__
node *                node_init ( const char *s, int val );
name_version_node_t * name_version_node_init(char *name, char *version, int val );
void free_node( node * n, char * which );
void free_node_cb( node * n );
void free_name_version_node( name_version_node_t * n, char * which);
#ifndef __HASHTABLE_H__
#include "hashtable.h"
#endif
size_t ht_kget_nval( hashtable_t *table, const char *str, char * which );
void ht_add_nvers_to_k_nval(hashtable_t *table, char *key, char *n, char *v, int val, char * which );
#endif
#ifdef __cplusplus
}
#endif
