/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_uuid.c
** @brief UUID operations
*/
#include <assert.h>
#include <ctype.h>
#include <string.h>

#ifndef INITGUID
#define INITGUID
#endif

#include <objbase.h>
#include <windows.h>

#include <libgf/gf_memory.h>
#include <libgf/gf_string.h>
#include <libgf/gf_array.h>
#include <libgf/gf_uuid.h>

#include "gf_local.h"

gf_status
gf_uuid_init(gf_uuid* uuid) {
  gf_validate(uuid);

  uuid->data_1 = 0;
  uuid->data_2 = 0;
  uuid->data_3 = 0;
  for (int i = 0; i < 8; i++) {
    uuid->data_4[i] = 0;
  }

  return GF_SUCCESS;
}

gf_status
gf_uuid_generate(gf_uuid* uuid) {
  GUID guid = { 0 };

  gf_validate(uuid);
  
  if (CoCreateGuid(&guid) != S_OK) {
    gf_raise(GF_E_API, "Failed to generate UUID.");
  }
  uuid->data_1 = guid.Data1;
  uuid->data_2 = guid.Data2;
  uuid->data_3 = guid.Data3;
  for (int i = 0; i < 8; i++) {
    uuid->data_4[i] = guid.Data4[i];
  }
  
  return GF_SUCCESS;
}

int
gf_uuid_compare(const gf_uuid* uuid_1, const gf_uuid* uuid_2) {
  assert(uuid_1);
  assert(uuid_2);
  
  if (uuid_1->data_1 > uuid_2->data_1) {
    return 1;
  } else if (uuid_1->data_1 < uuid_2->data_1) {
    return -1;
  } else {
    /* Do nothing */
  }
  if (uuid_1->data_2 > uuid_2->data_2) {
    return 1;
  } else if (uuid_1->data_2 < uuid_2->data_2) {
    return -1;
  } else {
    /* Do nothing */
  }
  if (uuid_1->data_3 > uuid_2->data_3) {
    return 1;
  } else if (uuid_1->data_3 < uuid_2->data_3) {
    return -1;
  } else {
    /* Do nothing */
  }
  for (int i = 0; i < 8; i++) {
    if (uuid_1->data_4[i] > uuid_2->data_4[i]) {
      return 1;
    } else if (uuid_1->data_4[i] < uuid_2->data_4[i]) {
      return -1;
    } else {
      /* Do nothing */
    }
  }
  
  return 0;
}

gf_status
gf_uuid_get_string(char* str, size_t len, const gf_uuid* uuid) {
  gf_validate(str);
  gf_validate(len >= GF_UUID_MAX);
  gf_validate(uuid);

  sprintf_s(
    str, len, "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
    uuid->data_1, uuid->data_2, uuid->data_3,
    uuid->data_4[0], uuid->data_4[1], uuid->data_4[2], uuid->data_4[3],
    uuid->data_4[4], uuid->data_4[5], uuid->data_4[6], uuid->data_4[7]);
  
  return 0;
}

#define uuid_parse_error(str) \
  gf_raise(GF_E_PARSE, "Invalid UUID string (%s)", str);

static gf_status
uuid_get_token(char* buf, size_t len, const char* ptr, size_t num) {
  size_t i = 0;
  
  gf_validate(len > num);

  for (i = 0; i < num; i++) {
    if (isxdigit((int)ptr[i])) {
      buf[i] = ptr[i];
    } else {
      uuid_parse_error(ptr);
    }
  }
  buf[i] = '\0';

  return GF_SUCCESS;
}

gf_status
gf_uuid_parse(gf_uuid* uuid, const char* str) {
  char token[GF_UUID_MAX] = { 0 };
  const char* ptr = NULL;
  char* end = NULL;
  size_t len = 0;
  
  gf_validate(uuid);
  gf_validate(!gf_strnull(str));

  /* Length check */
  len = gf_strlen(str);
  if (len != (GF_UUID_MAX - 1)) {
    uuid_parse_error(str);
  }

  /* separator */
  ptr = str;
  if (*ptr != '{') {
    uuid_parse_error(str);
  }
  ptr += 1;
  /* data 1 */
  _(uuid_get_token(token, GF_UUID_MAX, ptr, 8));
  uuid->data_1 = strtoul(token, &end, 16);
  if (*end != '\0') {
    uuid_parse_error(str);
  }
  ptr += 8;
  /* separator */
  if (*ptr != '-') {
    uuid_parse_error(str);
  }
  ptr += 1;
  /* data 2 */
  _(uuid_get_token(token, GF_UUID_MAX, ptr, 4));
  uuid->data_2 = (unsigned short)strtoul(token, &end, 16);
  if (*end != '\0') {
    uuid_parse_error(str);
  }
  ptr += 4;
  /* separator */
  if (*ptr != '-') {
    uuid_parse_error(str);
  }
  ptr += 1;
  /* data 3 */
  _(uuid_get_token(token, GF_UUID_MAX, ptr, 4));
  uuid->data_3 = (unsigned short)strtoul(token, &end, 16);
  if (*end != '\0') {
    uuid_parse_error(str);
  }
  ptr += 4;
  /* separator */
  if (*ptr != '-') {
    uuid_parse_error(str);
  }
  ptr += 1;
  /* data 4[0-1] */
  for (int i = 0; i < 2; i++) {
    _(uuid_get_token(token, GF_UUID_MAX, ptr, 2));
    uuid->data_4[i] = (unsigned char)strtoul(token, &end, 16);
    if (*end != '\0') {
      uuid_parse_error(str);
    }
    ptr += 2;
  }
  /* separator */
  if (*ptr != '-') {
    uuid_parse_error(str);
  }
  ptr += 1;
  /* data 4[2-7] */
  for (int i = 2; i < 8; i++) {
    _(uuid_get_token(token, GF_UUID_MAX, ptr, 2));
    uuid->data_4[i] = (unsigned char)strtoul(token, &end, 16);
    if (*end != '\0') {
      uuid_parse_error(str);
    }
    ptr += 2;
  }
  /* separator */
  if (*ptr != '}') {
    uuid_parse_error(str);
  }
  ptr += 1;
  /* NUL */
  if (*ptr != '\0') {
    uuid_parse_error(str);
  }
  
  return GF_SUCCESS;
}

/* -------------------------------------------------------------------------- */

struct gf_uuid_array {
  gf_array*       uuid_set;
  gf_uuid_free_fn free;
};

static gf_status
uuid_array_init(gf_uuid_array* ary) {
  gf_validate(ary);

  ary->uuid_set = NULL;
  ary->free = NULL;

  return GF_SUCCESS;
}

static gf_status
uuid_array_prepare(gf_uuid_array* ary) {
  gf_status rc = GF_SUCCESS;
  gf_validate(ary);

  rc = gf_array_new(&ary->uuid_set);
  gf_throw(rc);

  return GF_SUCCESS;
}

gf_status
gf_uuid_array_new(gf_uuid_array** ary) {
  gf_status rc = GF_SUCCESS;
  gf_uuid_array* tmp = NULL;

  rc = gf_malloc((gf_ptr *)&tmp, sizeof(*tmp));
  gf_throw(rc);
  
  rc = uuid_array_init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    gf_throw(rc);
  }
  rc = uuid_array_prepare(tmp);
  if (rc != GF_SUCCESS) {
    gf_uuid_array_free(tmp);
    gf_throw(rc);
  }
  *ary = tmp;
  
  return GF_SUCCESS;
}

void
gf_uuid_array_free(gf_uuid_array* ary) {
  if (ary) {
    (void)gf_uuid_array_clear(ary);
    gf_free(ary);
  }
}

gf_status
gf_uuid_array_clear(gf_uuid_array* ary) {
  gf_status rc = GF_SUCCESS;
  gf_size_t cnt = 0;

  gf_validate(ary);

  cnt = gf_uuid_array_count(ary);
  for (gf_size_t i = 0; i < cnt; i++) {
    gf_uuid* uuid = NULL;
    
    rc = gf_uuid_array_get(ary, i, &uuid);
    if (rc != GF_SUCCESS) {
      assert(0);
      continue;
    }
    if (ary->free) {
      ary->free(uuid);
    } else {
      gf_free(uuid);
    }
  }
  rc = gf_array_clear(ary->uuid_set);
  gf_throw(rc);

  return GF_SUCCESS;
}

gf_status
gf_uuid_array_set_free_fn(gf_uuid_array* ary, gf_uuid_free_fn fn) {
  gf_validate(ary);

  ary->free = fn;

  return GF_SUCCESS;
}

gf_status
gf_uuid_array_add(const gf_uuid_array* ary, gf_uuid* uuid) {
  gf_status rc = GF_SUCCESS;

  gf_validate(ary);
  gf_validate(uuid);

  rc = gf_array_add(ary->uuid_set, (gf_any){ .ptr = uuid });
  gf_throw(rc);

  return GF_SUCCESS;
}

gf_size_t
gf_uuid_array_count(const gf_uuid_array* ary) {
  return gf_array_size(ary->uuid_set);
}

gf_bool
gf_uuid_array_is_valid_index(const gf_uuid_array* ary, gf_size_t index) {
  gf_bool ret = GF_FALSE;
  gf_size_t cnt = 0;
  
  cnt = gf_uuid_array_count(ary);
  if (index < cnt) {
    ret = GF_TRUE;
  } else {
    ret = GF_FALSE;
  }

  return ret;
}

gf_status
gf_uuid_array_get(gf_uuid_array* ary, gf_size_t index, gf_uuid **uuid) {
  gf_status rc = GF_SUCCESS;
  gf_any any = { .data = 0 };
  
  gf_validate(ary);
  gf_validate(gf_uuid_array_is_valid_index(ary, index));
  gf_validate(uuid);

  rc = gf_array_get(ary->uuid_set, index, &any);
  gf_throw(rc);

  *uuid = (gf_uuid*)(any.ptr);
  
  return GF_SUCCESS;
}

gf_status
gf_uuid_array_find(gf_uuid_array* ary, const gf_uuid *key, gf_uuid **uuid) {
  gf_status rc = GF_SUCCESS;
  gf_size_t cnt = 0;
  gf_uuid* tmp = NULL;
  
  gf_validate(ary);
  gf_validate(key);
  gf_validate(uuid);

  cnt = gf_uuid_array_count(ary);
  for (gf_size_t i = 0; i < cnt; i++) {
    rc = gf_uuid_array_get(ary, i, &tmp);
    gf_throw(rc);
    if (!gf_uuid_compare(tmp, key)) {
      *uuid = tmp;
      return GF_SUCCESS;
    }
  }
  /* Not found */
  *uuid = NULL;

  return GF_SUCCESS;
}
