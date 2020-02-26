/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file test/test-shell.c
** @brief Testing module for gf_shell.
*/
#include <stdio.h>
#include <string.h>

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

static void
compare_same_files(void) {
  gf_status rc = 0;
  int ret = 0;
  gf_path* p1 = NULL;
  gf_path* p2 = NULL;
  
  static const char s[] = "THIS IS THE CONTENT OF FILES";
  static const char f1[] = "f1";
  static const char f2[] = "f2";
  
  FILE* fp1 = fopen(f1, "wb");
  CU_ASSERT_PTR_NOT_NULL(fp1);
  FILE* fp2 = fopen(f2, "wb");
  CU_ASSERT_PTR_NOT_NULL(fp2);

  fwrite(s, sizeof(char), strlen(s), fp1);
  fwrite(s, sizeof(char), strlen(s), fp2);

  fclose(fp1);
  fclose(fp2);

  rc = gf_path_new(&p1, f1);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_path_new(&p2, f2);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  /* TEST */
  ret = gf_shell_compare_files(p1, p2);
  CU_ASSERT_EQUAL(ret, 0); /* The same */

  /* Cleanup */
  rc = gf_shell_remove_file(p1);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_shell_remove_file(p2);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  
  gf_path_free(p1);
  gf_path_free(p2);
}

static void
compare_different_files(void) {
  gf_status rc = 0;
  int ret = 0;
  gf_path* p1 = NULL;
  gf_path* p2 = NULL;
  
  static const char s1[] = "THIS IS THE CONTENT OF FILES";
  static const char s2[] = "THIS IS THE CONTENT";
  static const char f1[] = "f1";
  static const char f2[] = "f2";
  
  FILE* fp1 = fopen(f1, "wb");
  CU_ASSERT_PTR_NOT_NULL(fp1);
  FILE* fp2 = fopen(f2, "wb");
  CU_ASSERT_PTR_NOT_NULL(fp2);

  fwrite(s1, sizeof(char), strlen(s1), fp1);
  fwrite(s2, sizeof(char), strlen(s2), fp2);

  fclose(fp1);
  fclose(fp2);

  rc = gf_path_new(&p1, f1);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_path_new(&p2, f2);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  /* TEST */
  ret = gf_shell_compare_files(p1, p2);
  CU_ASSERT_EQUAL(ret, 1); /* Different */

  /* Cleanup */
  rc = gf_shell_remove_file(p1);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_shell_remove_file(p2);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  
  gf_path_free(p1);
  gf_path_free(p2);
}

/* -------------------------------------------------------------------------- */

static void
copy_file_in_normal(void) {
  int ret = 0;
  gf_status rc = 0;
  gf_path* src = NULL;
  gf_path* dst = NULL;
  FILE* fp = NULL;

  /* Prepare */
  rc = gf_path_new(&src, "src");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_path_new(&dst, "dst");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  rc = gf_shell_touch(src);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  fp = fopen(gf_path_get_string(src), "w");
  CU_ASSERT_PTR_NOT_NULL_FATAL(fp);
  fprintf(fp, "THIS IS A TEST SOURCE FILE");
  fclose(fp);

  /* COPY TEST*/
  rc = gf_shell_copy_file(dst, src);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  ret = gf_shell_compare_files(dst, src);
  CU_ASSERT_EQUAL(ret, 0);

  /* Cleanup */
  rc = gf_shell_remove_file(src);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_shell_remove_file(dst);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  gf_path_free(src);
  gf_path_free(dst);
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
  /* file compare */
  CU_add_test(s, "compare the same files", compare_same_files);
  CU_add_test(s, "compare different files", compare_different_files);
  /* copy file */
  CU_add_test(s, "copy file in normal", copy_file_in_normal);
}
