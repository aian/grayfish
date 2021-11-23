/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file test/test-site.c
** @brief Testing module for gf_site.
*/
#include <CUnit/CUnit.h>

#include <libgf/gf_site.h>

#include "local.h"

/* -------------------------------------------------------------------------- */

static void
new_free_normal(void) {
  /*
  gf_status rc = 0;
  gf_site* site = NULL;

  rc = gf_site_new(&site);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  gf_site_free(site);
  */
}

/* -------------------------------------------------------------------------- */

/*!
** @brief The interface function for the test of gf_site.
**
** Registers the tests of gf_site module.
*/

void
gft_site_add_tests(void) {
  CU_pSuite s = CU_add_suite("Tests for gf_site", NULL, NULL);

  /* new/free */
  CU_add_test(s, "New/free in noraml case",   new_free_normal);
}
