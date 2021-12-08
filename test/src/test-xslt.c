/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file test/test-array.c
** @brief Testing module for gf_array.
*/
#include <CUnit/CUnit.h>

#include <libgf/gf_xslt.h>

#include "local.h"

#define GFT_TEST_SITE_ROOT GFT_TEST_DATA_PATH "/gf_xslt"

/* -------------------------------------------------------------------------- */

void
test_xslt_proc(void) {
  gf_status rc = 0;
  gf_xslt* xslt = NULL;
  gf_path* path = NULL;

  static const char xsl_path[] = GFT_TEST_SITE_ROOT "/style.xsl";
  static const char doc_path[] = GFT_TEST_SITE_ROOT "/doc.xml";
  static const char res_path[] = GFT_TEST_SITE_ROOT "/res.xml";

  rc = gf_xslt_new(&xslt);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  rc = gf_path_new(&path, xsl_path);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  rc = gf_xslt_read_template(xslt, path);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  rc = gf_path_set_string(path, doc_path);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_xslt_process(xslt, path);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  
  rc = gf_path_set_string(path, res_path);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_xslt_write_file(xslt, path);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  gf_path_free(path);
  
  gf_xslt_free(xslt);
}


/* -------------------------------------------------------------------------- */

/*!
** @brief The interface function for the test of gf_array.
**
** Registers the tests of gf_array module.
*/

void
gft_xslt_add_tests(void) {
  CU_pSuite s = CU_add_suite("Tests for gf_xslt", NULL, NULL);

  /* XSLT proc */
  CU_add_test(s, "XSLT proc", test_xslt_proc);
}
