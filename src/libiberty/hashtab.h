/* An expandable hash tables datatype.  Copyright (C) 1999, 2000, 2002, 2003, 2004, 2005, 2009, 2010
Free Software Foundation, Inc.  Contributed by Vladimir Makarov (vmakarov@cygnus.com).
This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */
#ifndef __HASHTAB_H__
#define __HASHTAB_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include "ansidecl.h"
#ifndef GTY
#define GTY(X)
#endif
  typedef unsigned int hashval_t;
  typedef hashval_t (*htab_hash) (const void *);
  typedef int (*htab_eq) (const void *, const void *);
  typedef void (*htab_del) (void *);
  typedef int (*htab_trav) (void **, void *);
  typedef void *(*htab_alloc) (size_t, size_t);
  typedef void (*htab_free) (void *);
  typedef void *(*htab_alloc_with_arg) (void *, size_t, size_t);
  typedef void (*htab_free_with_arg) (void *, void *);
#define HTAB_EMPTY_ENTRY    ((PTR) 0)
#define HTAB_DELETED_ENTRY  ((PTR) 1)
  struct GTY(()) htab {
    htab_hash hash_f;
    htab_eq eq_f;
    htab_del del_f;
    void ** GTY ((use_param, length ("%h.size"))) entries;
    size_t size;
    size_t n_elements;
    size_t n_deleted;
    unsigned int searches;
    unsigned int collisions;
    htab_alloc alloc_f;
    htab_free free_f;
    void * GTY((skip)) alloc_arg;
    htab_alloc_with_arg alloc_with_arg_f;
    htab_free_with_arg free_with_arg_f;
    unsigned int size_prime_index;
  };
  typedef struct htab *htab_t;
  enum insert_option {NO_INSERT, INSERT};
  extern htab_t  htab_create_alloc  (size_t, htab_hash, htab_eq, htab_del, htab_alloc, htab_free); 
  extern htab_t  htab_create_alloc_ex (size_t, htab_hash, htab_eq, htab_del, void *, htab_alloc_with_arg, htab_free_with_arg); 
  extern htab_t  htab_create_typed_alloc (size_t, htab_hash, htab_eq, htab_del, htab_alloc, htab_alloc, htab_free); 
  extern htab_t htab_create (size_t, htab_hash, htab_eq, htab_del);
  extern htab_t htab_try_create (size_t, htab_hash, htab_eq, htab_del);
  extern void  htab_set_functions_ex (htab_t, htab_hash, htab_eq, htab_del, void *, htab_alloc_with_arg, htab_free_with_arg);
  extern void  htab_delete (htab_t);
  extern void  htab_empty (htab_t);
  extern void *  htab_find (htab_t, const void *);
  extern void **  htab_find_slot (htab_t, const void *, enum insert_option);
  extern void *  htab_find_with_hash (htab_t, const void *, hashval_t);
  extern void **  htab_find_slot_with_hash (htab_t, const void *, hashval_t, enum insert_option);
  extern void  htab_clear_slot  (htab_t, void **);
  extern void  htab_remove_elt  (htab_t, void *);
  extern void  htab_remove_elt_with_hash (htab_t, void *, hashval_t);
  extern void  htab_traverse (htab_t, htab_trav, void *);
  extern void  htab_traverse_noresize (htab_t, htab_trav, void *);
  extern size_t  htab_size (htab_t);
  extern size_t  htab_elements (htab_t);
  extern double  htab_collisions  (htab_t);
  extern htab_hash htab_hash_pointer;
  extern htab_eq htab_eq_pointer;
  extern hashval_t htab_hash_string (const void *);
  extern hashval_t iterative_hash (const void *, size_t, hashval_t);
#define iterative_hash_object(OB,INIT) iterative_hash (&OB, sizeof (OB), INIT)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __HASHTAB_H */
