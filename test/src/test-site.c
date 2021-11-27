
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

#define GFT_TEST_SITE_ROOT GFT_TEST_DATA_PATH "/gf_site"

/* -------------------------------------------------------------------------- */

static void
new_free_normal(void) {
  gf_status rc = 0;
  gf_site* site = NULL;
  
  rc = gf_site_new(&site);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  
  gf_site_free(site);
}

static void
scan_website(void) {
  gf_status rc = 0;
  gf_site* site = NULL;
  gf_path* site_path = NULL;
  gf_path* site_file = NULL;

  static const char SITE_PATH[] = GFT_TEST_SITE_ROOT "/sample";
  static const char SITE_FILE[] = GFT_TEST_SITE_ROOT "/sample/site.xml";
  
  rc = gf_path_new(&site_path, SITE_PATH);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  
  rc = gf_site_scan(&site, site_path);
  gf_path_free(site_path);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);

  rc = gf_path_new(&site_file, SITE_FILE);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  rc = gf_site_write_file(site, site_file);
  gf_site_free(site);
  gf_path_free(site_file);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
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
  CU_add_test(s, "Scan a website",            scan_website);
}
