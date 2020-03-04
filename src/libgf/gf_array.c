/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_array.c
** @brief Variable array module.
*/
#include <assert.h>

#include <libgf/gf_swap.h>
#include <libgf/gf_memory.h>
#include <libgf/gf_array.h>

#include "gf_local.h"

#define ARRAY_CHUNK_SIZE 16

struct gf_array {
  gf_size_t used;
  gf_size_t size;
  gf_any*   data;
  /* Callbacks  */
  gf_array_free_fn free;
  gf_array_copy_fn copy;
};

static gf_status
array_init(gf_array* ary) {
  gf_validate(ary);

  ary->used = 0;
  ary->size = 0;
  ary->data = NULL;

  ary->free = NULL;
  ary->copy = NULL;
  
  return GF_SUCCESS;
}

static gf_bool
array_is_valid_index(const gf_array* ary, gf_size_t index) {
  if (!ary) {
    return GF_FALSE;
  }
  return (index < ary->used) ? GF_TRUE : GF_FALSE;
}

/*!
** @brief Helper function to remove element by index
**
** It removes specified element but does not shift (the blank and removable
** element remains).
**
** @param [in, out] ary   The array object
** @param [in, out] index The index of the element to remove
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

static void
array_remove_at(gf_array* ary, gf_size_t index) {
  assert(ary);
  assert(array_is_valid_index(ary, index));
  
  if (ary->free) {
    ary->free(&ary->data[index]);
  }
  ary->data[index].data = 0;
}

static gf_status
array_prepare(gf_array* ary) {
  gf_any* tmp = 0;
  
  gf_validate(ary);

  _(gf_malloc((gf_ptr*)&tmp, sizeof(*tmp) * ARRAY_CHUNK_SIZE));
  /* Initialize buffer */
  for (gf_size_t i = 0; i < ARRAY_CHUNK_SIZE; i++) {
    tmp[i].data = 0;
  }
  ary->data = tmp;
  ary->used = 0;
  ary->size = ARRAY_CHUNK_SIZE;

  ary->free = NULL;
  ary->copy = NULL;
  
  return GF_SUCCESS;
}

gf_status
gf_array_new(gf_array** ary) {
  gf_status rc = 0;
  gf_array* tmp = NULL;
  
  gf_validate(ary);

  _(gf_malloc((gf_ptr*)&tmp, sizeof(*tmp)));
  rc = array_init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    gf_throw(rc);
  }
  rc = array_prepare(tmp);
  if (rc != GF_SUCCESS) {
    gf_array_free(tmp);
    gf_throw(rc);
  }
  *ary = tmp;
  
  return GF_SUCCESS;
}

void
gf_array_free(gf_array* ary) {
  if (ary) {
    (void)gf_array_clear(ary);
    if (ary->data) {
      gf_free(ary->data);
      ary->data = NULL;
    }
    ary->used = 0;
    ary->size = 0;
    ary->free = NULL;
    ary->copy = NULL;

    gf_free(ary);
  }
}

gf_status
gf_array_clear(gf_array* ary) {
  gf_validate(ary);

  for (gf_size_t i = 0; i < ary->used; i++) {
    array_remove_at(ary, i);
  }
  ary->used = 0;

  return GF_SUCCESS;
}

gf_status
gf_array_swap(gf_array* lhs, gf_array* rhs) {
  gf_validate(lhs);
  gf_validate(rhs);

  gf_swap(lhs->data, rhs->data);
  gf_swap(lhs->used, rhs->used);
  gf_swap(lhs->size, rhs->size);
  gf_swap(lhs->free, rhs->free);
  gf_swap(lhs->copy, rhs->copy);
  
  return GF_SUCCESS;
}

gf_status
gf_array_set_free_fn(gf_array* ary, gf_array_free_fn fn) {
  gf_validate(ary);
  ary->free = fn;
  return GF_SUCCESS;
}

gf_status
gf_array_set_copy_fn(gf_array* ary, gf_array_copy_fn fn) {
  gf_validate(ary);
  ary->copy = fn;
  return GF_SUCCESS;
}

gf_status
gf_array_resize(gf_array* ary, gf_size_t size) {
  gf_size_t old_size = 0;
  gf_any* tmp = NULL;
  
  gf_validate(ary);

  old_size = ary->size;
  if (old_size < size) {
    /* Extend */
    _(gf_realloc((gf_ptr*)ary->data, sizeof(gf_any) * size));
    for (gf_size_t i = old_size; i < size; i++) {
      ary->data[i].data = 0;
    }
    ary->size = size;
  } else if (old_size > size) {
    /* Shrink */
    _(gf_malloc((gf_ptr*)&tmp, sizeof(*tmp) * size));
    for (gf_size_t i = 0; i < ary->size; i++) {
      if (i < ary->used) {
        tmp[i].data = ary->data[i].data;
      } else {
        array_remove_at(ary, i);
      }
    }
    gf_free(ary->data);
    ary->data = tmp;
    ary->size = size;
  } else {
    /* No change */
  }
  
  return GF_SUCCESS;
}

gf_status
gf_array_add(gf_array* ary, gf_any value) {
  gf_validate(ary);

  if (ary->used >= ary->size) {
    _(gf_array_resize(ary, ary->size + ARRAY_CHUNK_SIZE));
  }
  ary->data[ary->used].data = value.data;
  ary->used += 1;

  return GF_SUCCESS;
}

gf_status
gf_array_set(gf_array* ary, gf_size_t index, gf_any value) {
  gf_validate(ary);
  gf_validate(array_is_valid_index(ary, index));

  ary->data[index].data = value.data;
  
  return GF_SUCCESS;
}

gf_status
gf_array_get(const gf_array* ary, gf_size_t index, gf_any* value) {
  gf_validate(ary);
  gf_validate(value);
  gf_validate(array_is_valid_index(ary, index));

  value->data = ary->data[index].data;

  return GF_SUCCESS;
}

gf_status
gf_array_remove(gf_array* ary, gf_size_t index) {
  gf_size_t last = 0;
    
  gf_validate(ary);
  gf_validate(array_is_valid_index(ary, index));

  /* Remove element */
  array_remove_at(ary, index);
  /* Shift */
  last = ary->used - 1;
  for (gf_size_t i = index; i < last; i++) {
    gf_swap(ary->data[i], ary->data[i + 1]);
  }
  /* Update */
  ary->data[ary->used - 1].data = 0;
  ary->used -= 1;
  
  return GF_SUCCESS;
}

gf_size_t
gf_array_size(const gf_array* ary) {
  if (!ary) {
    return 0;
  }
  return ary->used;
}

gf_size_t
gf_array_buffer_size(const gf_array* ary) {
  if (!ary) {
    return 0;
  }
  return ary->size;
}
