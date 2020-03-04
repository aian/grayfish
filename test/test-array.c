/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file test/test-array.c
** @brief Testing module for gf_array.
*/
#include <CUnit/CUnit.h>

#include <libgf/gf_array.h>

#include "local.h"

/* -------------------------------------------------------------------------- */

static void
new_free_normal(void) {
  gf_status rc = 0;
  gf_array* ary = NULL;
  gf_size_t used = 0;
  gf_size_t size = 0;

  rc = gf_array_new(&ary);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  used = gf_array_size(ary);
  CU_ASSERT_EQUAL(used, 0);
  size = gf_array_buffer_size(ary);
  CU_ASSERT_EQUAL(size, 16);
    
  gf_array_free(ary);
}

static void
new_free_with_null(void) {
  gf_status rc = 0;

  rc = gf_array_new(NULL);
  CU_ASSERT_EQUAL(rc, GF_E_PARAM);
}

static void
add_get_normal(void) {
  gf_status rc = 0;
  gf_array* ary = NULL;
  gf_size_t used = 0;
  gf_size_t size = 0;
  gf_any any = { 0 };
  gf_any out = { 0 };

  rc = gf_array_new(&ary);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  any.u64 = 0xFEFE;
  rc = gf_array_add(ary, any);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  used = gf_array_size(ary);
  CU_ASSERT_EQUAL(used, 1);
  size = gf_array_buffer_size(ary);
  CU_ASSERT_EQUAL(size, 16);
  rc = gf_array_get(ary, 0, &out);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  CU_ASSERT_EQUAL(out.u64, 0xFEFE);

  gf_array_free(ary);
}



/* -------------------------------------------------------------------------- */

/*!
** @brief The interface function for the test of gf_array.
**
** Registers the tests of gf_array module.
*/

void
gft_array_add_tests(void) {
  CU_pSuite s = CU_add_suite("Tests for gf_array", NULL, NULL);

  /* new/free */
  CU_add_test(s, "New/free in noraml case",   new_free_normal);
  CU_add_test(s, "New/free with NULL",        new_free_with_null);
  /* add */
  CU_add_test(s, "Add/get element in normal", add_get_normal);
}
