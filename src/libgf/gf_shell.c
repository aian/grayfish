/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_shell.h
** @brief Shell operations.
*/

#include <assert.h>

#include <shlwapi.h>

#include <libgf/gf_shell.h>

gf_bool
gf_shell_is_file_exists(const gf_path* path) {
  const char* s = NULL;

  if (gf_path_is_empty(path)) {
    assert(GF_FALSE);
    return GF_FALSE;
  }
  s = gf_path_get_string(path);

  return path && PathFileExists(s) ? GF_TRUE : GF_FALSE;
}

struct gf_file_info {
  gf_path* path;
};

gf_status
gf_shell_get_file_info(gf_file_info** info, const gf_path* path) {
  gf_validate(info);
  gf_validate(!gf_path_is_empty(path));
  return GF_SUCCESS;
}

gf_bool
gf_shell_is_directory(const gf_path* path) {
  const char* s = NULL;

  if (gf_path_is_empty(path)) {
    assert(GF_FALSE);
    return GF_FALSE;
  }
  s = gf_path_get_string(path);
  
  return path && PathIsDirectory(path->buf) ? GF_TRUE : GF_FALSE;
}

gf_bool
gf_shell_is_normal_file(const gf_path* path) {
  gf_bool ret = GF_FALSE;

  if (gf_path_is_empty(path)) {
    assert(GF_FALSE);
    return GF_FALSE;
  }

  return ret;
}

gf_status
gf_shell_copy_file(const gf_path* dst, const gf_path* src) {
  gf_validate(!gf_path_is_empty(dst));
  gf_validate(!gf_path_is_empty(src));
  return GF_SUCCESS;
}

gf_status
gf_shell_make_directory(const gf_path* path) {
  gf_validate(!gf_path_is_empty(path));
  return GF_SUCCESS;
}

gf_status
gf_shell_touch(const gf_path* path) {
  gf_validate(!gf_path_is_empty(path));
  return GF_SUCCESS;
}

gf_status
gf_shell_remove_file(const gf_path* path) {
  gf_validate(!gf_path_is_empty(path));
  return GF_SUCCESS;
}

gf_status
gf_shell_remove_direcroty(const gf_path* path) {
  gf_validate(!gf_path_is_empty(path));
  return GF_SUCCESS;
}

gf_status
gf_shell_rename(const gf_path* dst, const gf_path* src) {
  gf_validate(!gf_path_is_empty(dst));
  gf_validate(!gf_path_is_empty(src));
  return GF_SUCCESS;
}

gf_status
gf_shell_move(const gf_path* dst, const gf_path* src) {
  return gf_shell_rename(dst, src);
}

gf_status
gf_shell_traverse_tree(
  const gf_path* path, gf_shell_traverse_order order, gf_shell_fn fn, gf_ptr data) {
  gf_validate(!gf_path_is_empty(path));
  assert(fn);

  (void)order;
  (void)data;
  
  return GF_SUCCESS;
}

gf_status
gf_shell_copy_tree(const gf_path* dst, const gf_path* src) {
  gf_validate(!gf_path_is_empty(dst));
  gf_validate(!gf_path_is_empty(src));

  return GF_SUCCESS;
}

gf_status
gf_shell_remove_tree(const gf_path* path) {
  gf_validate(!gf_path_is_empty(path));

  return GF_SUCCESS;
}

gf_status
gf_shell_change_directory(const gf_path* path) {
  gf_validate(!gf_path_is_empty(path));

  return GF_SUCCESS;
}
