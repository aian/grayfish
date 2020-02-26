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
create_file_in_normal(void) {
  gf_status rc = 0;
  gf_path* path = NULL;
  gf_bool ret = GF_FALSE;

  rc = gf_path_new(&path, "test-file");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  rc = gf_shell_touch(path);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  ret = gf_shell_file_exists(path);
  CU_ASSERT_EQUAL(ret, GF_TRUE);
  ret = gf_shell_is_normal_file(path);
  CU_ASSERT_EQUAL(ret, GF_TRUE);
  ret = gf_shell_is_directory(path);
  CU_ASSERT_EQUAL(ret, GF_FALSE);
  rc = gf_shell_remove_file(path);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);

  gf_path_free(path);
}

static void
create_file_with_null(void) {
  gf_status rc = 0;
  gf_path* path = NULL;
  gf_bool ret = GF_FALSE;

  rc = gf_path_new(&path, "");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  rc = gf_shell_touch(path);
  CU_ASSERT_NOT_EQUAL(rc, GF_SUCCESS);
  ret = gf_shell_is_normal_file(path);
  CU_ASSERT_EQUAL(ret, GF_FALSE);
  ret = gf_shell_is_directory(path);
  CU_ASSERT_EQUAL(ret, GF_FALSE);
  rc = gf_shell_remove_file(path);
  CU_ASSERT_NOT_EQUAL(rc, GF_SUCCESS);
  gf_path_free(path);

  rc = gf_shell_touch(NULL);
  CU_ASSERT_NOT_EQUAL(rc, GF_SUCCESS);
  ret = gf_shell_is_normal_file(path);
  CU_ASSERT_EQUAL(ret, GF_FALSE);
  ret = gf_shell_is_directory(path);
  CU_ASSERT_EQUAL(ret, GF_FALSE);
  rc = gf_shell_remove_file(NULL);
  CU_ASSERT_NOT_EQUAL(rc, GF_SUCCESS);
}

static void
create_directory_in_normal(void) {
  gf_status rc = 0;
  gf_path* path = NULL;

  rc = gf_path_new(&path, "dir");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  rc = gf_shell_make_directory(path);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  rc = gf_shell_remove_directory(path);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);

  gf_path_free(path);
}

static void
create_directory_with_null(void) {
  gf_status rc = 0;
  gf_path* path = NULL;

  rc = gf_path_new(&path, "");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  rc = gf_shell_make_directory(path);
  CU_ASSERT_NOT_EQUAL(rc, GF_SUCCESS);

  rc = gf_shell_remove_directory(path);
  CU_ASSERT_NOT_EQUAL(rc, GF_SUCCESS);

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
  CU_pSuite s = CU_add_suite("Tests for gf_shell", shell_init, shell_cleanup);

  /* create and remove */
  CU_add_test(s, "create file in normal", create_file_in_normal);
  CU_add_test(s, "create file with null", create_file_with_null);
  CU_add_test(s, "create directory in normal", create_directory_in_normal);
  CU_add_test(s, "create directory with null", create_directory_with_null);
  
}
