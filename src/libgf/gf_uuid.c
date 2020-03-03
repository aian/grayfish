/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_uuid.c
** @brief UUID operations
*/
#include <assert.h>

#ifndef INITGUID
#define INITGUID
#endif

#include <objbase.h>
#include <windows.h>

#include <libgf/gf_string.h>
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

gf_status
gf_uuid_parse(gf_uuid* uuid, const char* str) {
  // TODO: implement me !!
  (void)uuid;
  (void)str;
  return GF_SUCCESS;
}
