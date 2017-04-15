#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "aht.h"
static int ht_expand_if_needed(struct hashtable *t);
static unsigned int next_power(unsigned int size);
static int ht_insert(struct hashtable *t, void *key, unsigned int *avail_index);
static struct ht_ele *ht_free_element = (void*) -1;
u_int32_t djb_hash(unsigned char *buf, size_t len) {
  u_int32_t h = 5381;
  while(len--)
    h = (h + (h << 5)) ^ *buf++;
  return h;
}
u_int32_t djb_hashR(unsigned char *buf, size_t len) {
  u_int32_t h = 5381;
  buf += len-1;
  while(len--)
    h = (h + (h << 5)) ^ *buf--;
  return h;
}
/* Another trivial hash function */
#define ROT32R(x,n) (((x)>>n)|(x<<(32-n)))
u_int32_t trivial_hash(unsigned char *buf, size_t len) {
  u_int32_t h = 0;
  while(len--) {
    h = h + *buf++;
    h = ROT32R(h, 3);
  }
  return h;
}
u_int32_t trivial_hashR(unsigned char *buf, size_t len) {
  u_int32_t h = 0;
  buf += len-1;
  while(len--) {
    h = h + *buf--;
    h = ROT32R(h, 3);
  }
  return h;
}
#define mix(a,b,c) { \
  a=a-b; a=a-c; a=a^(c>>13); \
  b=b-c; b=b-a; b=b^(a<<8); \
  c=c-a; c=c-b; c=c^(b>>13); \
  a=a-b; a=a-c; a=a^(c>>12); \
  b=b-c; b=b-a; b=b^(a<<16); \
  c=c-a; c=c-b; c=c^(b>>5); \
  a=a-b; a=a-c; a=a^(c>>3); \
  b=b-c; b=b-a; b=b^(a<<10); \
  c=c-a; c=c-b; c=c^(b>>15); \
}
u_int32_t __ht_strong_hash(u_int8_t *k, u_int32_t length, u_int32_t initval) {
  u_int32_t a,b,c; /* the internal state */
  u_int32_t len; /* how many key bytes still need mixing */

  /* Set up the internal state */
  len = length;
  a = b = 0x9e3779b9; /* the golden ratio; an arbitrary value */
  c = initval; /* variable initialization of internal state */

  /*---------------------------------------- handle most of the key */
  while (len >= 12)
  {
    a=a+(k[0]+((u_int32_t)k[1]<<8)+((u_int32_t)k[2]<<16)+
        ((u_int32_t)k[3]<<24));
    b=b+(k[4]+((u_int32_t)k[5]<<8)+((u_int32_t)k[6]<<16)+
        ((u_int32_t)k[7]<<24));
    c=c+(k[8]+((u_int32_t)k[9]<<8)+((u_int32_t)k[10]<<16)+
        ((u_int32_t)k[11]<<24));
    mix(a,b,c);
    k = k+12; len = len-12;
  }

  /*------------------------------------- handle the last 11 bytes */
  c = c+length;
  switch(len) /* all the case statements fall through */
  {
    case 11: c=c+((u_int32_t)k[10]<<24);
    case 10: c=c+((u_int32_t)k[9]<<16);
    case 9 : c=c+((u_int32_t)k[8]<<8);
             /* the first byte of c is reserved for the length */
    case 8 : b=b+((u_int32_t)k[7]<<24);
    case 7 : b=b+((u_int32_t)k[6]<<16);
    case 6 : b=b+((u_int32_t)k[5]<<8);
    case 5 : b=b+k[4];
    case 4 : a=a+((u_int32_t)k[3]<<24);
    case 3 : a=a+((u_int32_t)k[2]<<16);
    case 2 : a=a+((u_int32_t)k[1]<<8);
    case 1 : a=a+k[0];
             /* case 0: nothing left to add */
  }
  mix(a,b,c);
  /*-------------------------------------------- report the result */
  return c;
}
static void ht_reset(struct hashtable *t) {
  t->table = NULL;
  t->size = 0;
  t->sizemask = 0;
  t->used = 0;
  t->collisions = 0;
}
int ht_init(struct hashtable *t) {
  ht_reset(t);
  t->hashf = ht_hash_pointer;
  t->key_destructor = ht_no_destructor;
  t->val_destructor = ht_no_destructor;
  t->key_compare = ht_compare_ptr;
  return HT_OK;
}
int ht_resize(struct hashtable *t) {
  int minimal = (t->used * 2)+1;

  if (minimal < HT_INITIAL_SIZE)
    minimal = HT_INITIAL_SIZE;
  return ht_expand(t, minimal);
}
int ht_move(struct hashtable *orig, struct hashtable *dest, unsigned int index) {
  int ret;
  unsigned int new_index;

  /* If the element isn't in the table ht_search will store
   * the index of the free ht_ele in the integer pointer by *index */
  ret = ht_insert(dest, orig->table[index]->key, &new_index);
  if (ret != HT_OK)
    return ret;

  /* Move the element */
  dest->table[new_index] = orig->table[index];
  orig->table[index] = ht_free_element;
  orig->used--;
  dest->used++;
  return HT_OK;
}
int ht_expand(struct hashtable *t, size_t size) {
  struct hashtable n; /* the new hashtable */
  unsigned int realsize = next_power(size), i;
  if (t->used >= size)
    return HT_INVALID;
  ht_init(&n);
  n.size = realsize;
  n.sizemask = realsize-1;
  n.table = malloc(realsize*sizeof(struct ht_ele*));
  if (n.table == NULL)
    return HT_NOMEM;
  n.hashf = t->hashf;
  n.key_destructor = t->key_destructor;
  n.val_destructor = t->val_destructor;
  n.key_compare= t->key_compare;
  memset(n.table, 0, realsize*sizeof(struct ht_ele*));
  n.used = t->used;
  for (i = 0; i < t->size && t->used > 0; i++) {
    if (t->table[i] != NULL && t->table[i] != ht_free_element) {
      u_int32_t h;
      h = n.hashf(t->table[i]->key) & n.sizemask;
      if (n.table[h]) {
        n.collisions++;
        while(1) {
          h = (h+1) & n.sizemask;
          if (!n.table[h])
            break;
          n.collisions++;
        }
      }
      n.table[h] = t->table[i];
      t->used--;
    }
  }
  assert(t->used == 0);
  free(t->table);
  *t = n;
  return HT_OK;
}
int ht_replace(struct hashtable *t, void *key, void *data) {
  int ret;
  unsigned int index;

  /* Try to add the element */
  ret = ht_add(t, key, data);
  if (ret == HT_OK || ret != HT_BUSY)
    return ret;
  /* It already exists, get the index */
  ret = ht_search(t, key, &index);
  assert(ret == HT_FOUND);
  /* Remove the old */
  ret = ht_free(t, index);
  assert(ret == HT_OK);
  /* And add the new */
  return ht_add(t, key, data);
}
int ht_add(struct hashtable *t, void *key, void *data) {
  int ret;
  unsigned int index;
  ret = ht_insert(t, key, &index);
  if (ret != HT_OK)
    return ret;
  if ((t->table[index] = malloc(sizeof(struct ht_ele))) == NULL)
    return HT_NOMEM;
  t->table[index]->key = key;
  t->table[index]->data = data;
  t->used++;
  return HT_OK;
}
int ht_rm(struct hashtable *t, void *key) {
  int ret;
  unsigned int index;

  if ((ret = ht_search(t, key, &index)) != HT_FOUND)
    return ret;
  return ht_free(t, index);
}
int ht_destroy(struct hashtable *t) {
  unsigned int i;

  /* Free all the elements */
  for (i = 0; i < t->size && t->used > 0; i++) {
    if (t->table[i] != NULL && t->table[i] != ht_free_element) {
      if (t->key_destructor)
        t->key_destructor(t->table[i]->key);
      if (t->val_destructor)
        t->val_destructor(t->table[i]->data);
      free(t->table[i]);
      t->used--;
    }
  }
  /* Free the table and the allocated cache structure */
  free(t->table);
  /* Re-initialize the table */
  ht_reset(t);
  return HT_OK; /* Actually ht_destroy never fails */
}
int ht_free(struct hashtable *t, unsigned int index) {
  if (index >= t->size)
    return HT_IOVERFLOW; /* Index overflow */
  /* ht_free() calls against non-existent elements are ignored */
  if (t->table[index] != NULL && t->table[index] != ht_free_element) {
    /* release the key */
    if (t->key_destructor)
      t->key_destructor(t->table[index]->key);
    /* release the value */
    if (t->val_destructor)
      t->val_destructor(t->table[index]->data);
    /* free the element structure */
    free(t->table[index]);
    /* mark the element as freed */
    t->table[index] = ht_free_element;
    t->used--;
  }
  return HT_OK;
}
int ht_search(struct hashtable *t, void *key, unsigned int *found_index) {
  int ret;
  u_int32_t h;

  /* Expand the hashtable if needed */
  if (t->size == 0) {
    if ((ret = ht_expand_if_needed(t)) != HT_OK)
      return ret;
  }

  /* Try using the first hash functions */
  h = t->hashf(key) & t->sizemask;
  /* this handles the removed elements */
  if (!t->table[h])
    return HT_NOTFOUND;
  if (t->table[h] != ht_free_element &&
      t->key_compare(key, t->table[h]->key))
  {
    *found_index = h;
    return HT_FOUND;
  }

  while(1) {
    h = (h+1) & t->sizemask;
    /* this handles the removed elements */
    if (t->table[h] == ht_free_element)
      continue;
    if (!t->table[h])
      return HT_NOTFOUND;
    if (t->key_compare(key, t->table[h]->key)) {
      *found_index = h;
      return HT_FOUND;
    }
  }
}
int ht_get_byindex(struct hashtable *t, unsigned int index) {
  if (index >= t->size)
    return -1;
  if (t->table[index] == NULL || t->table[index] == ht_free_element)
    return 0;
  return 1;
}
void **ht_get_array(struct hashtable *t) {
  int used = ht_used(t);
  void **table, **tptr;
  long idx;

  if ((table = (void**) malloc(sizeof(void*)*(used*2))) == NULL)
    return NULL;
  tptr = table;
  for (idx = 0; ;idx++) {
    int type = ht_get_byindex(t, idx);
    if (type == -1) break;
    if (type == 0) continue;
    *tptr++ = ht_key(t, idx);
    *tptr++ = ht_value(t, idx);
  }
  return table;
}
static int ht_expand_if_needed(struct hashtable *t) {
  /* If the hash table is empty expand it to the intial size,
   * if the table is half-full redobule its size. */
  if (t->size == 0)
    return ht_expand(t, HT_INITIAL_SIZE);
  if (t->size <= t->used*2)
    return ht_expand(t, t->size * 2);
  return HT_OK;
}
static unsigned int next_power(unsigned int size) {
  unsigned int i = 256;

  if (size >= 2147483648U)
    return 2147483648U;
  while(1) {
    if (i >= size)
      return i;
    i *= 2;
  }
}
static int ht_insert(struct hashtable *t, void *key, unsigned int *avail_index) {
  int ret;
  u_int32_t h;

  /* Expand the hashtable if needed */
  if ((ret = ht_expand_if_needed(t)) != HT_OK)
    return ret;

  /* Try using the first hash functions */
  h = t->hashf(key) & t->sizemask;
  /* this handles the removed elements */
  if (!t->table[h] || t->table[h] == ht_free_element) {
    *avail_index = h;
    return HT_OK;
  }
  t->collisions++;
  if (t->key_compare(key, t->table[h]->key))
    return HT_BUSY;

  while(1) {
    h = (h+1) & t->sizemask;
    /* this handles the removed elements */
    if (!t->table[h] || t->table[h] == ht_free_element) {
      *avail_index = h;
      return HT_OK;
    }
    t->collisions++;
    if (t->key_compare(key, t->table[h]->key))
      return HT_BUSY;
  }
}
void ht_destructor_free(void *obj) {
  free(obj);
}
int ht_compare_ptr(void *key1, void *key2) {
  return (key1 == key2);
}
int ht_compare_string(void *key1, void *key2) {
  return (strcmp(key1, key2) == 0) ? 1 : 0;
}
static u_int32_t strong_hash_init_val = 0xF937A21;
void ht_set_strong_hash_init_val(u_int32_t secret) {
  strong_hash_init_val = secret;
}
u_int32_t ht_strong_hash(u_int8_t *k, u_int32_t length, u_int32_t initval) {
  return __ht_strong_hash(k, length, initval^strong_hash_init_val);
}
u_int32_t ht_hash_string(void *key) {
  return __ht_strong_hash(key, strlen(key), strong_hash_init_val);
}
u_int32_t ht_hash_pointer(void *key) {
  return __ht_strong_hash((void*)&key, sizeof(void*), strong_hash_init_val);
}
