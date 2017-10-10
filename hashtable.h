// @file   hashtable.h @author Andrea Guzzo @date   22/09/2013 @brief  Fast thread-safe hashtable implementation @note   In case of failures reported from the pthread interface abort() will be called. Callers can catch SIGABRT if more actions need to be taken.  
#ifndef HL_HASHTABLE_H
#define HL_HASHTABLE_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <stdbool.h>
#include "linklist.h"
typedef struct _hashtable_s hashtable_t;
typedef void (*ht_free_item_callback_t)(void *);
#define HT_SIZE_MIN 128
hashtable_t* ht_create(size_t initial_size, size_t max_size, ht_free_item_callback_t free_item_cb);
int ht_init(hashtable_t *table, size_t initial_size, size_t max_size, ht_free_item_callback_t free_item_cb);
void ht_set_free_item_callback(hashtable_t *table, ht_free_item_callback_t cb);
void ht_clear(hashtable_t *table);
void ht_destroy(hashtable_t *table);
void* ht_get(hashtable_t *table, void *key, size_t klen, size_t *dlen);
int ht_exists(hashtable_t *table, void *key, size_t klen);
void* ht_get_copy(hashtable_t *table, void *key, size_t klen, size_t *dlen);
typedef void *(*ht_deep_copy_callback_t)(void *data, size_t dlen, void *user);
void *ht_get_deep_copy(hashtable_t *table, void *key, size_t klen, size_t *dlen, ht_deep_copy_callback_t copy_cb, void *user);
int ht_set(hashtable_t *table, void *key, size_t klen, void *data, size_t dlen);
int ht_get_and_set(hashtable_t *table, void *key, size_t klen, void *data, size_t dlen, void **prev_data, size_t *prev_len);
int ht_get_or_set(hashtable_t *table, void *key, size_t klen, void *data, size_t dlen, void **cur_data, size_t *cur_len);
int ht_set_copy(hashtable_t *table, void *key, size_t klen, void *data, size_t dlen, void **prev_data, size_t *prev_len);
int ht_set_if_not_exists(hashtable_t *table, void *key, size_t klen, void *data, size_t dlen);
int ht_set_if_equals(hashtable_t *table, void *key, size_t klen, void *data, size_t dlen, void *match, size_t match_size, void **prev_data, size_t *prev_len);
int ht_unset(hashtable_t *table, void *key, size_t klen, void **prev_data, size_t *prev_len);
int ht_delete(hashtable_t *table, void *key, size_t klen, void **prev_data, size_t *prev_len);
int ht_delete_if_equals(hashtable_t *table, void *key, size_t klen, void *match, size_t match_size);
typedef int (*ht_pair_callback_t)(hashtable_t *table, void *key, size_t klen, void **value, size_t *vlen, void *user);
int ht_call(hashtable_t *table, void *key, size_t klen, ht_pair_callback_t cb, void *user);
size_t ht_count(hashtable_t *table);
typedef struct _hashtable_key_s {
    void  *data;
    size_t len;
    size_t vlen;
} hashtable_key_t;
linked_list_t *ht_get_all_keys(hashtable_t *table);
typedef struct _hashtable_value_s {
    void *key;
    size_t klen;
    void  *data;
    size_t len;
} hashtable_value_t;
linked_list_t *ht_get_all_values(hashtable_t *table);
typedef enum {
    HT_ITERATOR_STOP = 0,
    HT_ITERATOR_CONTINUE = 1,
    HT_ITERATOR_REMOVE = -1,
    HT_ITERATOR_REMOVE_AND_STOP = -2
} ht_iterator_status_t;
typedef ht_iterator_status_t (*ht_key_iterator_callback_t)(hashtable_t *table, void *key, size_t klen, void *user);
void ht_foreach_key(hashtable_t *table, ht_key_iterator_callback_t cb, void *user);
typedef ht_iterator_status_t (*ht_value_iterator_callback_t)(hashtable_t *table, void *value, size_t vlen, void *user);
void ht_foreach_value(hashtable_t *table, ht_value_iterator_callback_t cb, void *user);
typedef ht_iterator_status_t (*ht_pair_iterator_callback_t)(hashtable_t *table, void *key, size_t klen, void *value, size_t vlen, void *user);
void ht_foreach_pair(hashtable_t *table, ht_pair_iterator_callback_t cb, void *user);
#ifdef __cplusplus
}
#endif
#endif
