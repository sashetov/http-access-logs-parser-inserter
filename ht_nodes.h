#define ERROR_MAX               100
#define HT_ALLOC_SIZE_MAX       4048
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
static int  nodes_equal( const void *entry, const void *element );
static void node_delete( node *n );
#ifndef __HASHTAB_H__
#include "hashtab.h"
#endif
#ifndef __HASHTABLE_H__
#include "hashtable.h"
#endif
static hashval_t ht_get_node_hash( const void *n );
void node_ht_init( htab_t *table, size_t size );
static void key_del( void *key );
size_t ht_kget_nval( hashtable_t *table, const char *str );
void ht_kadd_val_to_k_nval( hashtable_t * table, const char *str, int val);
void ht_insert_next_val( hashtable_t *table, const char *srt );
#endif
