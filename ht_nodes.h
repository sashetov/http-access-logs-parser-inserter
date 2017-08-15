#ifdef __cplusplus
extern "C" {
#endif
#define ERROR_MAX               100
#define HT_ALLOC_SIZE_MAX       40480
#define HT_ALLOC_SIZE_DEFAULT   1024
#include <stdlib.h>
#include <stdint.h>
#ifndef __HT_NODE__
typedef struct node {
  char  *name;
  size_t nval;
} node;
#define __HT_NODE__
node        *node_init ( const char *s, int val );
void free_node( node * n);
#ifndef __HASHTABLE_H__
#include "hashtable.h"
#endif
size_t ht_kget_nval( hashtable_t *table, const char *str );
void ht_kadd_val_to_k_nval( hashtable_t * table, const char *str, int val);
#endif
#ifdef __cplusplus
}
#endif
