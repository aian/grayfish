/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_date.h
** @brief Datetime management
*/
#include <libgf/gf_memory.h>
#include <libgf/gf_date.h>

#include "gf_local.h"

/*
** file_time::u32[0] == FILETIME::dwLowDateTime
** file_time::u32[1] == FILETIME::dwHighDateTime
*/

union gf_date {
  gf_any file_time;
};

gf_status
date_init(gf_date* date) {
  gf_validate(date);

  date->file_time.data = 0;

  return GF_SUCCESS;
}

gf_status
gf_date_new(gf_date** date) {
  gf_status rc = 0;
  gf_date* tmp = NULL;

  gf_validate(date);
  
  _(gf_malloc((gf_ptr*)&date, sizeof(*tmp)));

  rc = date_init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    gf_throw(rc);
  }

  *date = tmp;
  
  return GF_SUCCESS;
}

void
gf_date_free(gf_date* date) {
  if (date) {
    gf_free(date);
  }
}

