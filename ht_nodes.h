#define ERROR_MAX          10240
#define LINE_MAX           1000000
#define CONFIG_DEBUG       1
#define CONFIG_STREAM_MODE 0
#define CONFIG_TIME_DELTA  0
#define HT_ALLOC_SIZE_MAX  1000000
#include <stdlib.h>
#include <stdint.h>
#ifndef ___HT_NODE__
typedef struct node {
  char  *name;
  size_t nval;
} node;
#define ___HT_NODE__
node        *node_init ( const char *s );
static int  nodes_equal( const void *entry, const void *element );
static void node_delete( node *n );
#ifndef __HASHTAB_H__
#include "hashtab.h"
#endif
static hashval_t ht_get_node_hash( const void *n );
void ht_init( htab_t *table, int size );
static void key_del( void *key );
size_t ht_kget_nval( htab_t table, const char *str );
void ht_kadd_val_to_k_nval( htab_t table, const char *str, size_t nval);
#endif
