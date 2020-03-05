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

/* -------------------------------------------------------------------------- */

const gf_int GF_STRING_INVALID_CHAR = -1;

struct gf_string {
  gf_size_t used;
  gf_size_t size;
  gf_char*  data;
};

static gf_bool
string_is_valid_index(const gf_string* str, gf_size_t index) {
  return str && index < str->used ? GF_TRUE : GF_FALSE;
}

static gf_status
string_init(gf_string* str) {
  gf_validate(str);

  str->used = 0;
  str->size = 0;
  str->data = 0;

  return GF_SUCCESS;
}

gf_status
gf_string_new(gf_string** str) {
  gf_status rc = 0;
  gf_string* tmp = 0;

  gf_validate(str);
  
  _(gf_malloc((gf_ptr*)&tmp, sizeof(*tmp)));

  rc = string_init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    gf_throw(rc);
  }

  *str = tmp;

  return GF_SUCCESS;
}

void
gf_string_free(gf_string* str) {
  if (str) {
    if (str->data) {
      gf_free(str->data);
    }
    (void)string_init(str);
    gf_free(str);
  }
}

gf_status
gf_string_set(gf_string* str, const gf_char* s) {
  gf_size_t len = 0;
  
  gf_validate(str);
  gf_validate(s);

  _(gf_strassign(&str->data, s));
  len =  gf_strlen(s) + 1;
  str->used = len;
  str->size = len;
  
  return GF_SUCCESS;
}

const gf_char*
gf_string_get(const gf_string* str) {
  return str ? str->data : NULL;
}

gf_size_t
gf_string_size(const gf_string* str) {
  return str ? str->used : 0;
}

gf_int
gf_string_get_at(const gf_string* str, gf_size_t index) {
  if (!str || !string_is_valid_index(str, index)) {
    return GF_STRING_INVALID_CHAR;
  }
  return str->data[index];
}
