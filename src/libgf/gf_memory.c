/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_memory.c
** @brief Memory management.
*/
#include <stdlib.h>
#include <string.h>

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

gf_status
gf_memset(gf_ptr buf, gf_int ch, gf_size_t n) {
  gf_validate(buf);

  memset(buf, ch, n);

  return GF_SUCCESS;
}

gf_status
gf_memcpy(gf_ptr dst, gf_const_ptr src, gf_size_t n) {
  gf_validate(dst);
  gf_validate(src);
  
  if (n > 0) {
    memcpy(dst, src, n);
  }

  return GF_SUCCESS;
}


gf_status
gf_bzero(gf_ptr buf, gf_size_t n) {
  return gf_memset(buf, 0, n);
}
