// @file linklist.h @author Andrea Guzzo @date 22/09/2013 @brief Fast thread-safe linklist implementation @note   In case of failures reported from the pthread interface abort() will be called. Callers can catch SIGABRT if more actions need to be taken.  
#ifndef HL_LINKLIST_H
#define HL_LINKLIST_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdlib.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <sys/types.h>
#ifdef WIN32
#ifdef THREAD_SAFE
#include <w32_pthread.h>
#endif
#endif
#include <string.h>
typedef void (*free_value_callback_t)(void *v);
typedef int (*list_comparator_callback_t)(void *v1, void *v2);
typedef struct _list_entry_s {
  struct _linked_list_s *list;
  struct _list_entry_s *prev;
  struct _list_entry_s *next;
  void *value;
  int tagged;
} list_entry_t;
typedef struct _linked_list_s {
  list_entry_t *head;
  list_entry_t *tail;
  list_entry_t *cur;
  size_t  pos;
  size_t length;
#ifdef THREAD_SAFE
  pthread_mutex_t lock;
#endif
  free_value_callback_t free_value_cb;
  int refcnt;
  list_entry_t *slices;
} linked_list_t;
linked_list_t *list_create();
int list_init(linked_list_t *list);
void list_destroy(linked_list_t *list);
void list_clear(linked_list_t *list);
size_t list_count(linked_list_t *list);
void list_set_free_value_callback(linked_list_t *list, free_value_callback_t free_value_cb);
void list_lock(linked_list_t *list);
void list_unlock(linked_list_t *list);
void *list_pop_value(linked_list_t *list);
int list_push_value(linked_list_t *list, void *val);
int list_unshift_value(linked_list_t *list, void *val);
void *list_shift_value(linked_list_t *list);
int list_insert_value(linked_list_t *list, void *val, size_t pos);
void *list_set_value(linked_list_t *list, size_t pos, void *val);
void *list_subst_value(linked_list_t *list, size_t pos, void *val);
void *list_pick_value(linked_list_t *list, size_t pos);
void *list_fetch_value(linked_list_t *list, size_t pos);
int list_move_value(linked_list_t *list, size_t srcPos, size_t dstPos);
int list_swap_values(linked_list_t *list, size_t pos1, size_t pos2);
typedef int (*item_handler_t)(void *item, size_t idx, void *user);
int list_foreach_value(linked_list_t *list, item_handler_t item_handler, void *user);
typedef struct _tagged_value_s {
    char *tag;
    void *value;
    size_t vlen;
    char type;
#define TV_TYPE_STRING 0
#define TV_TYPE_BINARY 1
#define TV_TYPE_LIST   2
} tagged_value_t;
tagged_value_t *list_create_tagged_value(char *tag, void *val, size_t len);
tagged_value_t *list_create_tagged_value_nocopy(char *tag, void *val);
tagged_value_t *list_create_tagged_sublist(char *tag, linked_list_t *list);
void list_destroy_tagged_value(tagged_value_t *tval);
tagged_value_t *list_pop_tagged_value(linked_list_t *list);
int list_push_tagged_value(linked_list_t *list, tagged_value_t *tval);
int list_unshift_tagged_value(linked_list_t *list, tagged_value_t *tval);
tagged_value_t *list_shift_tagged_value(linked_list_t *list);
int list_insert_tagged_value(linked_list_t *list, tagged_value_t *tval, size_t pos);
tagged_value_t *list_pick_tagged_value(linked_list_t *list, size_t pos);
tagged_value_t *list_fetch_tagged_value(linked_list_t *list, size_t pos);
tagged_value_t *list_get_tagged_value(linked_list_t *list, char *tag);
tagged_value_t *list_set_tagged_value(linked_list_t *list, char *tag, void *value, size_t len, int copy);
size_t list_get_tagged_values(linked_list_t *list, char *tag, linked_list_t *values);
void list_sort(linked_list_t *list, list_comparator_callback_t comparator);
typedef struct  _slice_s slice_t;
slice_t *slice_create(linked_list_t *list, size_t offset, size_t length);
void slice_destroy(slice_t *slice);
int slice_foreach_value(slice_t *slice, item_handler_t item_handler, void *user);
#ifdef __cplusplus
}
#endif
#endif
