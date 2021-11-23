/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file test/test-file_info.c
** @brief Testing module for gf_file_info.
*/
#include <CUnit/CUnit.h>

#include <libgf/gf_file_info.h>

#include "local.h"

/* -------------------------------------------------------------------------- */

static void
new_free_normal(void) {
  /*
  gf_status rc = 0;
  gf_file_info* info = NULL;

  rc = gf_file_info_new(&info);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  gf_file_info_free(info);
  */
}

/* -------------------------------------------------------------------------- */

/*!
** @brief The interface function for the test of gf_file_info.
**
** Registers the tests of gf_file_info module.
*/

void
gft_file_info_add_tests(void) {
  CU_pSuite s = CU_add_suite("Tests for gf_file_info", NULL, NULL);

  /* new/free */
  CU_add_test(s, "New/free in noraml case",   new_free_normal);
}

