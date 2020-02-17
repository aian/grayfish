/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file test/test-path.c
** @brief Testing gf_path module.
*/
#include <CUnit/CUnit.h>

#include <libgf/gf_path.h>

#include "local.h"

/* -------------------------------------------------------------------------- */

static void
new_free_with_path_string(void) {
  gf_status rc = 0;
  gf_path* path = NULL;

  rc = gf_path_new(&path, "file");
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);

  gf_path_free(path);
}

static void
new_free_with_empty_string(void) {
  gf_status rc = 0;
  gf_path* path = NULL;

  rc = gf_path_new(&path, "");
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);

  gf_path_free(path);
}

static void
new_free_with_null_ptr(void) {
  gf_status rc = 0;
  gf_path* path = NULL;

  rc = gf_path_new(&path, NULL);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);

  gf_path_free(path);
}

/* -------------------------------------------------------------------------- */

static void
get_string_with_path_string(void) {
  gf_status rc = 0;
  gf_path* path = NULL;
  const char* str = NULL;

  static const char s[] = "file";

  rc = gf_path_new(&path, s);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  str = gf_path_get_string(path);
  CU_ASSERT_STRING_EQUAL(str, s);
  
  gf_path_free(path);
}

static void
get_string_with_null_ptr(void) {
  gf_status rc = 0;
  gf_path* path = NULL;
  const char* str = NULL;

  rc = gf_path_new(&path, NULL);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  str = gf_path_get_string(path);
  CU_ASSERT_STRING_EQUAL(str, "");
  
  gf_path_free(path);
}

/* -------------------------------------------------------------------------- */

void
gft_path_add_tests() {
  CU_pSuite s = CU_add_suite("gf_path", NULL, NULL);

  /* create and destroy */
  CU_add_test(s, "New/free with path string",   new_free_with_path_string);
  CU_add_test(s, "New/free with empty string",  new_free_with_empty_string);
  CU_add_test(s, "New/free with null ptr",      new_free_with_null_ptr);
  /* get string */
  CU_add_test(s, "Get string with path string", get_string_with_path_string);
  CU_add_test(s, "Get string with null ptr",    get_string_with_null_ptr);
}
