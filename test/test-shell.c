/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file test/test-shell.c
** @brief Testing module for gf_shell.
*/
#include <CUnit/CUnit.h>

#include <libgf/gf_shell.h>

#include "util.h"
#include "local.h"


static gft_test_ctxt* ctxt_ = NULL;

/* -------------------------------------------------------------------------- */

static void
create_remove_file_in_normal(void) {
  gf_status rc = 0;
  gf_path* path = NULL;

  rc = gf_path_new(&path, "test-file");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  rc = gf_shell_touch(path);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  rc = gf_shell_remove_file(path);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);

  gf_path_free(path);
}

/* -------------------------------------------------------------------------- */

/*!
** @brief Initialize the test suite
*/

static int
shell_init(void) {
  int rc = 0;
  
  assert(ctxt_ == NULL);

  rc = gft_test_ctxt_new(&ctxt_);
  if (rc != 0) {
    return rc;
  }
  
  return 0;
}

/*!
** @brief Cleanup the test suite
*/

static int
shell_cleanup(void) {
  if (ctxt_) {
    gft_test_ctxt_free(ctxt_);
    ctxt_ = NULL;
  }
  return 0;
}

/*!
** @brief The interface function for the test of gf_path.
**
** Registers the tests of gf_path module.
*/

void
gft_shell_add_tests(void) {
  CU_pSuite s = CU_add_suite("Tests for gf_path", shell_init, shell_cleanup);

  /* create and remove */
  CU_add_test(s, "create/remove file in normal", create_remove_file_in_normal);
}
