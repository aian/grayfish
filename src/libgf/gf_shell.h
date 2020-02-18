/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_shell.h
** @brief Shell operations.
*/
#ifndef LIBGF_GF_SHELL_H
#define LIBGF_GF_SHELL_H

#pragma once

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>
#include <libgf/gf_path.h>

#ifdef __cplusplus
extern "C" {
#endif


extern gf_bool gf_shell_is_file_exists(const gf_path* path);

typedef struct gf_file_info gf_file_info;

extern gf_status gf_shell_get_file_info(gf_file_info** info, const gf_path* path);

extern gf_bool gf_shell_is_directory(const gf_path* path);
extern gf_bool gf_shell_is_normal_file(const gf_path* path);


extern gf_status gf_shell_copy_file(const gf_path* dst, const gf_path* src);
extern gf_status gf_shell_make_directory(const gf_path* path);
extern gf_status gf_shell_touch(const gf_path* path);
extern gf_status gf_shell_remove_file(const gf_path* path);
extern gf_status gf_shell_remove_direcroty(const gf_path* path);
extern gf_status gf_shell_rename(const gf_path* path);

typedef gf_status (*gf_shell_fn)(const gf_path* path, gf_ptr data);

enum gf_shell_traverse_order {
  GF_SHELL_TRAVERSE_PREORDER  = 0,
  GF_SHELL_TRAVERSE_POSTORDER = 1,
};

typedef enum gf_shell_traverse_order gf_shell_traverse_order;

extern gf_status gf_shell_traverse_tree(
  const gf_path* path, gf_shell_traverse_order order, gf_shell_fn fn, gf_ptr data);
extern gf_status gf_shell_copy_tree(const gf_path* dst, const gf_path* src);
extern gf_status gf_shell_remove_tree(const gf_path* path);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_SHELL_H */
