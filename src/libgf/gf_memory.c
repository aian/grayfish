/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_memory.c
** @brief Memory management.
*/
#include <stdlib.h>

#define GF_USE_SAFE_ERROR_ 1

#include <libgf/gf_memory.h>

gf_status
gf_malloc(gf_ptr* ptr, gf_size_t size) {
  gf_ptr tmp = NULL;
  
  gf_validate(ptr);
  gf_validate(size > 0);

  tmp = malloc(size);
  if (!tmp) {
    gf_raise(GF_E_ALLOC, "Memory allocation error.");
  }
  *ptr = tmp;

  return GF_SUCCESS;
}

gf_status
gf_realloc(gf_ptr* ptr, gf_size_t size) {
  gf_ptr* tmp = NULL;

  gf_validate(ptr);
  gf_validate(size > 0);

  tmp = realloc(*ptr, size);
  if (!tmp) {
    gf_raise(GF_E_ALLOC, "Memory re-allocation error");
  }
  *ptr = tmp;

  return GF_SUCCESS;
}

void
gf_free(gf_ptr ptr) {
  if (ptr) {
    free(ptr);
  }
}
