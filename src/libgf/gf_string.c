/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_string.h
** @brief String operation.
*/
#include <string.h>

#include <libgf/gf_memory.h>
#include <libgf/gf_string.h>
#include <libgf/gf_local.h>

gf_bool
gf_strnull(const char* str) {
  return str && str[0] ? GF_FALSE : GF_TRUE;
}

gf_size_t
gf_strlen(const char* str) {
  return gf_strnull(str) ? 0 : strlen(str);
}

gf_status
gf_strdup(char** dst, const char* src) {
  gf_size_t len = 0;
  gf_size_t bufsize = 0;
  char* str = 0;
  
  gf_validate(dst);
  gf_validate(src);

  len = gf_strlen(src);
  bufsize = len + 1;
  
  _(gf_malloc((gf_ptr *)&str, bufsize));
  str[len] = '\0';
  memcpy_s(str, bufsize, src, len);
  *dst = str;
  
  return GF_SUCCESS;
}

gf_status
gf_strassign(char** dst, const char* src) {
  char* str = 0;
  
  gf_validate(dst);
  gf_validate(src);

  _(gf_strdup(&str, src));
  if (*dst) {
    gf_free(*dst);
    *dst = NULL;
  }
  *dst = str;

  return GF_SUCCESS;
}
