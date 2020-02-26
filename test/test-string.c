/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file test/test-string.c
** @brief Testing module for gf_string.
*/
#include <CUnit/CUnit.h>

#include <libgf/gf_memory.h>
#include <libgf/gf_string.h>

#include "local.h"

/* -------------------------------------------------------------------------- */

void
test_strnull(void) {
  gf_bool ret = GF_FALSE;

  ret = gf_strnull("hello");
  CU_ASSERT_EQUAL(ret, GF_FALSE);

  ret = gf_strnull("");
  CU_ASSERT_EQUAL(ret, GF_TRUE);

  ret = gf_strnull(NULL);
  CU_ASSERT_EQUAL(ret, GF_TRUE);
}

/* -------------------------------------------------------------------------- */

void
test_strlen(void) {
  size_t len = 0;

  len = gf_strlen("hello");
  CU_ASSERT_EQUAL(len, 5);

  len = gf_strlen("");
  CU_ASSERT_EQUAL(len, 0);

  len = gf_strlen(NULL);
  CU_ASSERT_EQUAL(len, 0);
}

/* -------------------------------------------------------------------------- */

void
test_strdup(void) {
  gf_status rc = 0;
  char* s = NULL;
  static const char str[] = "string";
  static const char* null = "";

  rc = gf_strdup(&s, "sample");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  s = NULL;
  rc = gf_strassign(&s, str);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  CU_ASSERT_STRING_EQUAL(s, str);
  CU_ASSERT_PTR_NOT_EQUAL(s, str);
  gf_free(s);

  s = NULL;
  rc = gf_strassign(&s, null);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  CU_ASSERT_STRING_EQUAL(s, null);
  CU_ASSERT_PTR_NOT_EQUAL(s, null);
  gf_free(s);
  
  s = NULL;
  rc = gf_strdup(&s, NULL);
  CU_ASSERT_NOT_EQUAL(rc, GF_SUCCESS);
  CU_ASSERT_PTR_EQUAL(s, NULL);
}

/* -------------------------------------------------------------------------- */

void
test_strassign(void) {
  gf_status rc = 0;
  char* s = NULL;
  static const char str[] = "string";
  static const char null[] = "";


  s = NULL;
  rc = gf_strdup(&s, str);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  CU_ASSERT_STRING_EQUAL(s, str);
  CU_ASSERT_PTR_NOT_EQUAL(s, str);
  gf_free(s);

  s = NULL;
  rc = gf_strdup(&s, null);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  CU_ASSERT_STRING_EQUAL(s, null);
  CU_ASSERT_PTR_NOT_EQUAL(s, null);
  gf_free(s);
  
  s = NULL;
  rc = gf_strdup(&s, NULL);
  CU_ASSERT_NOT_EQUAL(rc, GF_SUCCESS);
  CU_ASSERT_PTR_EQUAL(s, NULL);
}

/* -------------------------------------------------------------------------- */

/*!
** @brief The interface function for the test of gf_string.
**
** Registers the tests of gf_string module.
*/

void
gft_string_add_tests(void) {
  CU_pSuite s = CU_add_suite("Tests for gf_string", NULL, NULL);
  /* gf_strnull() */
  CU_add_test(s, "gf_strnull",   test_strnull);
  /* gf_strlen() */
  CU_add_test(s, "gf_strlen",    test_strlen);
  /* gf_strdup() */
  CU_add_test(s, "gf_strdup",    test_strdup);
  /* gf_strassign() */
  CU_add_test(s, "gf_strassign", test_strassign);
}
