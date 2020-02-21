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

/*!
** @brief Test if the specified file exists
**
** @param [in] path The path to test if the file exists
**
** @return GF_TRUE if the file exists, GF_FALSE otherwise.
*/

extern gf_bool gf_shell_file_exists(const gf_path* path);

/*!
** @brief File information type
*/

typedef struct gf_file_info gf_file_info;

/*!
** @brief Get the information of the specified file
**
** @param info [out] The file information of the specified file.
** @param path [in]  The path to the file to get information
**
** @return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_shell_get_file_info(gf_file_info** info, const gf_path* path);

/*!
** @brief Test if the specified file is a directory
**
** @param path [in]  The path to the file
**
** @return GF_TRUE if the file is a directory, GF_FALSE otherwise.
*/

extern gf_bool gf_shell_is_directory(const gf_path* path);

/*!
** @brief Test if the specified file is a normal file
**
** @param path [in]  The path to the file
**
** @return GF_TRUE if the file is a normal file, GF_FALSE otherwise.
*/

extern gf_bool gf_shell_is_normal_file(const gf_path* path);

/*!
** @brief Copy file
**
** @param dst [in] The path to the file to which is copied
** @param src [in] The path to the file from which is copied
**
** @return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_shell_copy_file(const gf_path* dst, const gf_path* src);

/*!
** @brief Create directory
**
** @param path [in] The path to the directory to be created
**
** @return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_shell_make_directory(const gf_path* path);

/*!
** @brief Create an empty file
**
** @param path [in] The path to the file to be created
**
** @return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_shell_touch(const gf_path* path);

/*!
** @brief Remove the specified file
**
** @param path [in] The path to the file to be removed
**
** @return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_shell_remove_file(const gf_path* path);

/*!
** @brief Remove the specified directory
**
** @note This function does remove an empty directory. If you want to remove
** whole directory tree, call gf_shell_remove_tree() instead.
**
** @param path [in] The path to the directory to be removed
**
** @return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_shell_remove_directory(const gf_path* path);

/*!
** @brief Rename the name of specified file
**
** @param dst [in] The path to the file or directory to which is renamed
** @param src [in] The path to the file or directory from which is renamed
**
** @return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_shell_rename(const gf_path* dst, const gf_path* src);

/*!
** @brief The alias for the function <code>gf_shell_rename()</code>.
**
** This function is the alias for the function <code>gf_shell_rename()</code>.
**
** @param dst [in] The path to the file or directory to which is moved
** @param src [in] The path to the file or directory from which is moved
**
** @return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_shell_move(const gf_path* dst, const gf_path* src);

/*!
t** @brief Function pointer type for shell operations
**
** This type is used for automated shell operations. For instance, in the
** function <code>gf_shell_traverse_tree()</code>, you can specify this type of
** function. And the function is called when it reaches a file node as a
** callback.
*/

typedef gf_status (*gf_shell_fn)(
  const gf_path* path, const gf_path* trace, gf_ptr find_data, gf_ptr data);

/*!
** @brief The order of evaluation in traversing directory tree.
*/

enum gf_shell_traverse_order {
  GF_SHELL_TRAVERSE_PREORDER  = 0,  ///< Evaluate node in preorder
  GF_SHELL_TRAVERSE_POSTORDER = 1,  ///< Evaluate node in postorder
};

/*!
** @brief The typedef of the <code>enum gf_shell_traverse_order</code>.
*/

typedef enum gf_shell_traverse_order gf_shell_traverse_order;

/*!
** @brief Traverse the directory tree recursively
**
** @param path       [in] The path to be traversed.
** @param trace_path [in] The path which is actually traced
** @param order      [in] The order when the specifed callback is called.
** @param fn         [in] The pointer to the callback function.
** @param data       [in, out] The user data with which the calllback is called.
**
** @return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_shell_traverse_tree(
  const gf_path* path, const gf_path* trace_path, gf_shell_traverse_order order,
  gf_shell_fn fn, gf_ptr data);

/*!
** @brief Copy a file or a directory recursively
**
** @param dst [in] The path to the file or directory to which is to be copied
** @param src [in] The path to the file or directory from which is to becopied
**
** @return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_shell_copy_tree(const gf_path* dst, const gf_path* src);

/*!
** @brief Remove a file or a directory recursively
**
** @param path [in] The path to the file or directory to be removed.
**
** @return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_shell_remove_tree(const gf_path* path);

/* -------------------------------------------------------------------------- */

/*!
** @brief Change the current directory
**
** @param [in] path The path to which the current directory is changed
**
** @return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_shell_change_directory(const gf_path* path);


#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_SHELL_H */
