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

static void
copy_file_with_null(void) {
  gf_status rc = 0;
  gf_path* path = NULL;
  gf_path* null = NULL;
  FILE* fp = NULL;

  /* Prepare */
  rc = gf_path_new(&path, "file");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_path_new(&null, "");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  rc = gf_shell_touch(path);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  fp = fopen(gf_path_get_string(path), "w");
  CU_ASSERT_PTR_NOT_NULL_FATAL(fp);
  fprintf(fp, "THIS IS A TEST SOURCE FILE");
  fclose(fp);

  /* COPY TEST*/
  rc = gf_shell_copy_file(path, null);
  CU_ASSERT_NOT_EQUAL(rc, GF_SUCCESS);
  rc = gf_shell_copy_file(path, NULL);
  CU_ASSERT_NOT_EQUAL(rc, GF_SUCCESS);
  rc = gf_shell_copy_file(null, path);
  CU_ASSERT_NOT_EQUAL(rc, GF_SUCCESS);
  rc = gf_shell_copy_file(NULL, path);
  CU_ASSERT_NOT_EQUAL(rc, GF_SUCCESS);

  /* Cleanup */
  rc = gf_shell_remove_file(path);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  gf_path_free(path);
  gf_path_free(null);
}

/* -------------------------------------------------------------------------- */

static void
rename_file_in_normal(void) {
  gf_bool ret = 0;
  gf_status rc = 0;
  gf_path* src_1 = NULL;
  gf_path* src_2 = NULL;
  gf_path* dst_1 = NULL;
  gf_path* dst_2 = NULL;

  /* Prepare */
  rc = gf_path_new(&src_1, "src_1");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_path_new(&src_2, "src_2");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_path_new(&dst_1, "dst_1");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_path_new(&dst_2, "dst_2");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  rc = gf_shell_touch(src_1);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_shell_touch(src_2);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  
  /* RENAME TEST*/
  rc = gf_shell_rename(dst_1, src_1);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  ret = gf_shell_file_exists(src_1);
  CU_ASSERT_EQUAL(ret, GF_FALSE);
  ret = gf_shell_file_exists(dst_1);
  CU_ASSERT_EQUAL(ret, GF_TRUE);

  rc = gf_shell_move(dst_2, src_2);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  ret = gf_shell_file_exists(src_1);
  CU_ASSERT_EQUAL(ret, GF_FALSE);
  ret = gf_shell_file_exists(dst_1);
  CU_ASSERT_EQUAL(ret, GF_TRUE);
  
  /* Cleanup */
  rc = gf_shell_remove_file(dst_1);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_shell_remove_file(dst_2);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  gf_path_free(src_1);
  gf_path_free(src_2);
  gf_path_free(dst_1);
  gf_path_free(dst_2);
}

static void
rename_directory_in_normal(void) {
  gf_bool ret = 0;
  gf_status rc = 0;
  gf_path* src_1 = NULL;
  gf_path* src_2 = NULL;
  gf_path* dst_1 = NULL;
  gf_path* dst_2 = NULL;

  /* Prepare */
  rc = gf_path_new(&src_1, "src_1");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_path_new(&src_2, "src_2");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_path_new(&dst_1, "dst_1");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_path_new(&dst_2, "dst_2");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  rc = gf_shell_make_directory(src_1);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_shell_make_directory(src_2);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  
  /* RENAME TEST*/
  rc = gf_shell_rename(dst_1, src_1);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  ret = gf_shell_file_exists(src_1);
  CU_ASSERT_EQUAL(ret, GF_FALSE);
  ret = gf_shell_file_exists(dst_1);
  CU_ASSERT_EQUAL(ret, GF_TRUE);

  rc = gf_shell_move(dst_2, src_2);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  ret = gf_shell_file_exists(src_1);
  CU_ASSERT_EQUAL(ret, GF_FALSE);
  ret = gf_shell_file_exists(dst_1);
  CU_ASSERT_EQUAL(ret, GF_TRUE);
  
  /* Cleanup */
  rc = gf_shell_remove_directory(dst_1);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_shell_remove_directory(dst_2);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  gf_path_free(src_1);
  gf_path_free(src_2);
  gf_path_free(dst_1);
  gf_path_free(dst_2);
}

/* -------------------------------------------------------------------------- */

void
copy_tree_in_normal(void) {
  gf_status rc = 0;
  gf_bool ret = GF_FALSE;
  gf_path* s_dir1 = NULL;
  gf_path* s_dir2 = NULL;
  gf_path* s_file = NULL;

  gf_path* d_dir1 = NULL;
  gf_path* d_dir2 = NULL;
  gf_path* d_file = NULL;

  rc = gf_path_new(&s_dir1, "d1");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_path_new(&s_dir2, "d1\\d2");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_path_new(&s_file, "d1\\d2\\f");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  rc = gf_path_new(&d_dir1, "dst");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_path_new(&d_dir2, "dst\\d2");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_path_new(&d_file, "dst\\d2\\f");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  
  rc = gf_shell_make_directory(s_dir1);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_shell_make_directory(s_dir2);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_shell_touch(s_file);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  /* TEST */
  rc = gf_shell_copy_tree(d_dir1, s_dir1);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);

  ret = gf_shell_file_exists(d_dir1);
  CU_ASSERT_EQUAL(ret, GF_TRUE);
  ret = gf_shell_is_directory(d_dir1);
  CU_ASSERT_EQUAL(ret, GF_TRUE);

  ret = gf_shell_file_exists(d_dir2);
  CU_ASSERT_EQUAL(ret, GF_TRUE);
  ret = gf_shell_is_directory(d_dir2);
  CU_ASSERT_EQUAL(ret, GF_TRUE);

  ret = gf_shell_file_exists(d_file);
  CU_ASSERT_EQUAL(ret, GF_TRUE);
  ret = gf_shell_is_normal_file(d_file);
  CU_ASSERT_EQUAL(ret, GF_TRUE);
  
  /* Cleanup */
  gf_shell_remove_file(s_file);
  gf_shell_remove_directory(s_dir2);
  gf_shell_remove_directory(s_dir1);

  gf_shell_remove_file(d_file);
  gf_shell_remove_directory(d_dir2);
  gf_shell_remove_directory(d_dir1);

  gf_path_free(s_dir1);
  gf_path_free(s_dir2);
  gf_path_free(s_file);

  gf_path_free(d_dir1);
  gf_path_free(d_dir2);
  gf_path_free(d_file);
}

/* -------------------------------------------------------------------------- */

void
remove_tree_in_normal(void) {
  gf_status rc = 0;
  gf_path* s_dir1 = NULL;
  gf_path* s_dir2 = NULL;
  gf_path* s_file = NULL;

  rc = gf_path_new(&s_dir1, "d1");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_path_new(&s_dir2, "d1\\d2");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_path_new(&s_file, "d1\\d2\\f");
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  
  rc = gf_shell_make_directory(s_dir1);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_shell_make_directory(s_dir2);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);
  rc = gf_shell_touch(s_file);
  CU_ASSERT_EQUAL_FATAL(rc, GF_SUCCESS);

  /* TEST */
  rc = gf_shell_remove_tree(s_dir1);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  
  /* Cleanup */
  gf_path_free(s_dir1);
  gf_path_free(s_dir2);
  gf_path_free(s_file);
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
  CU_add_test(s, "copy file with null", copy_file_with_null);
  /* rename file */
  CU_add_test(s, "rename file in normal", rename_file_in_normal);
  CU_add_test(s, "rename directory in normal", rename_directory_in_normal);
  /* copy tree */
  CU_add_test(s, "copy tree in normal", copy_tree_in_normal);
  /* remove tree */
  CU_add_test(s, "remove tree in normal", remove_tree_in_normal);
}
