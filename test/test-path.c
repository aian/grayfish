/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file test/test-path.c
** @brief Testing module for gf_path.
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
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  str = gf_path_get_string(path);
  CU_ASSERT_STRING_EQUAL(str, s);
  CU_ASSERT_PTR_NOT_EQUAL(str, s);  // path string is copied deeply
  
  gf_path_free(path);
}

static void
get_string_with_null_ptr(void) {
  gf_status rc = 0;
  gf_path* path = NULL;
  const char* str = NULL;

  rc = gf_path_new(&path, NULL);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  str = gf_path_get_string(path);
  CU_ASSERT_STRING_EQUAL(str, "");
  
  gf_path_free(path);
}

/* -------------------------------------------------------------------------- */

static void
set_string_with_path_string(void) {
  gf_status rc = 0;
  gf_path* path = NULL;
  const char* str = NULL;

  static const char s1[] = "file_1";
  static const char s2[] = "file_2";

  rc = gf_path_new(&path, s1);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  str = gf_path_get_string(path);
  CU_ASSERT_STRING_EQUAL_FATAL(str, s1);

  rc = gf_path_set_string(path, s2);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  str = gf_path_get_string(path);
  CU_ASSERT_STRING_EQUAL(str, s2);
  CU_ASSERT_PTR_NOT_EQUAL(str, s2);  // path string is copied deeply
  
  gf_path_free(path);
}

static void
set_string_with_null_ptr(void) {
  gf_status rc = 0;
  gf_path* path = NULL;
  const char* str = NULL;

  static const char s1[] = "file_1";

  rc = gf_path_new(&path, s1);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  str = gf_path_get_string(path);
  CU_ASSERT_STRING_EQUAL_FATAL(str, s1);

  rc = gf_path_set_string(path, NULL);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  str = gf_path_get_string(path);
  CU_ASSERT_STRING_EQUAL(str, "");
  
  gf_path_free(path);
}

/* -------------------------------------------------------------------------- */

static void
swap_paths_in_normal_cond(void) {
  gf_status rc = 0;
  gf_path* path_1 = NULL;
  gf_path* path_2 = NULL;
  const char* str_1 = NULL;
  const char* str_2 = NULL;

  static const char s1[] = "file_1";
  static const char s2[] = "file_2";

  rc = gf_path_new(&path_1, s1);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_path_new(&path_2, s2);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  rc = gf_path_swap(path_1, path_2);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  str_1 = gf_path_get_string(path_1);
  str_2 = gf_path_get_string(path_2);
  CU_ASSERT_PTR_NOT_NULL_FATAL(str_1);
  CU_ASSERT_PTR_NOT_NULL_FATAL(str_2);
  CU_ASSERT_STRING_EQUAL(str_1, s2);
  CU_ASSERT_STRING_EQUAL(str_2, s1);
  
  gf_path_free(path_1);
  gf_path_free(path_2);
}

static void
swap_paths_with_null_ptr(void) {
  gf_status rc = 0;
  gf_path* path = NULL;
  const char* str = NULL;

  static const char s[] = "file_1";

  rc = gf_path_new(&path, s);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  rc = gf_path_swap(path, NULL);
  CU_ASSERT_NOT_EQUAL(rc, GF_SUCCESS);  // fails
  str = gf_path_get_string(path);
  CU_ASSERT_STRING_EQUAL(str, s);       // no effect
  
  rc = gf_path_swap(NULL, path);
  CU_ASSERT_NOT_EQUAL(rc, GF_SUCCESS);  // fails
  str = gf_path_get_string(path);
  CU_ASSERT_STRING_EQUAL(str, s);       // no effect
  
  gf_path_free(path);
}

/* -------------------------------------------------------------------------- */

static void
copy_paths_in_normal_cond(void) {
  gf_status rc = 0;
  gf_path* path_1 = NULL;
  gf_path* path_2 = NULL;
  const char* str_1 = NULL;
  const char* str_2 = NULL;

  static const char s[] = "file";

  rc = gf_path_new(&path_1, s);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_path_new(&path_2, "");   // empty string
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  rc = gf_path_copy(path_2, path_1);  // copy 1 --> 2
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);

  str_1 = gf_path_get_string(path_1);
  str_2 = gf_path_get_string(path_2);
  CU_ASSERT_PTR_NOT_NULL_FATAL(str_1);
  CU_ASSERT_PTR_NOT_NULL_FATAL(str_2);

  CU_ASSERT_STRING_EQUAL(str_1, str_2);
  CU_ASSERT_PTR_NOT_EQUAL(str_1, str_2);
  
  gf_path_free(path_1);
  gf_path_free(path_2);
}

static void
copy_paths_with_null_ptr(void) {
  gf_status rc = 0;
  gf_path* path = NULL;
  const char* str = NULL;

  static const char s[] = "file";

  rc = gf_path_new(&path, s);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  rc = gf_path_copy(path, NULL);
  CU_ASSERT_NOT_EQUAL(rc, GF_SUCCESS);
  str = gf_path_get_string(path);
  CU_ASSERT_STRING_EQUAL(str, s);

  rc = gf_path_copy(NULL, path);
  CU_ASSERT_NOT_EQUAL(rc, GF_SUCCESS);
  str = gf_path_get_string(path);
  CU_ASSERT_STRING_EQUAL(str, s);

  gf_path_free(path);
}

/* -------------------------------------------------------------------------- */

/*!
** @brief The interface function for the test of gf_path.
**
** Registers the tests of gf_path module.
*/

void
gft_path_add_tests(void) {
  CU_pSuite s = CU_add_suite("Tests for gf_path", NULL, NULL);

  /* create and destroy */
  CU_add_test(s, "New/free with path string",   new_free_with_path_string);
  CU_add_test(s, "New/free with empty string",  new_free_with_empty_string);
  CU_add_test(s, "New/free with null ptr",      new_free_with_null_ptr);
  /* get string */
  CU_add_test(s, "Get string with path string", get_string_with_path_string);
  CU_add_test(s, "Get string with null ptr",    get_string_with_null_ptr);
  /* set string */
  CU_add_test(s, "Set string with path string", set_string_with_path_string);
  CU_add_test(s, "Set string with null ptr",    set_string_with_null_ptr);
  /* swap */
  CU_add_test(s, "Swap paths in normal cond",   swap_paths_in_normal_cond);
  CU_add_test(s, "Swap paths with null ptr",    swap_paths_with_null_ptr);
  /* copy */
  CU_add_test(s, "Copy paths in normal cond",   copy_paths_in_normal_cond);
  CU_add_test(s, "Copy paths with null ptr",    copy_paths_with_null_ptr);
}
