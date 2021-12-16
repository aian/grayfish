/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_file_info.h
** @brief Operating file information.
*/
#ifndef LIBGF_GF_FILE_INFO_H
#define LIBGF_GF_FILE_INFO_H

#pragma once

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>

#include <libgf/gf_path.h>

#ifdef __cplusplus
extern {
#endif

typedef struct gf_file_info gf_file_info;

/*!
** @brief Create a new gf_file_info object.
**
** This function creates a new gf_file_info object. This function get
** information of single file or directory . If you want to get that of a whole
** directory tree, use gf_file_info_scan() instead.
**
** @param [out] info The pointer to the pointer of a new gf_file_info object.
** @param [in]  disp_path The path for display.
** @param [in]  path A path of the file from which you want to get information.
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/
extern gf_status gf_file_info_new(
  gf_file_info** info, const gf_path* disp_path, const gf_path* path);

extern gf_status gf_file_info_scan(gf_file_info** info, const gf_path* path);

/*!
** @brief Discards the gf_file_info object.
**
** @param [in, out] info 
*/
extern void gf_file_info_free(gf_file_info* info);

extern void gf_file_info_free_any(gf_any* any);

extern gf_status gf_file_info_copy(gf_file_info* dst, const gf_file_info* src);

extern gf_status gf_file_info_clone(gf_file_info** dst, const gf_file_info* src);

extern gf_bool gf_file_info_is_file(const gf_file_info* info);
extern gf_bool gf_file_info_is_directory(const gf_file_info* info);

extern gf_size_t gf_file_info_count_children(const gf_file_info* info);

extern gf_status gf_file_info_add_child(
  gf_file_info* info, gf_file_info* child);

extern gf_status gf_file_info_get_child(
  const gf_file_info* info, gf_size_t index, gf_file_info** child);

/* -------------------------------------------------------------------------- */

extern gf_status gf_file_info_get_file_name(
  const gf_file_info* info, const gf_char** file_name);

extern gf_bool gf_file_info_does_file_name_equal(
  const gf_file_info* info, const gf_char* file_name);

extern gf_status gf_file_info_get_full_path(
  const gf_file_info* info, const gf_char** full_path);

extern gf_status gf_file_info_get_hash(
  const gf_file_info* info, gf_size_t size, gf_8u* hash);

extern gf_status gf_file_info_get_hash_string(
  const gf_file_info* info, gf_size_t size, gf_8u* str);

extern gf_status gf_file_info_get_user_data(
  const gf_file_info* info, gf_any* user_data);

extern gf_status gf_file_info_get_user_flag(
  const gf_file_info* info, gf_32u* user_flag);

extern gf_status gf_file_info_get_hash_size(
  const gf_file_info* info, gf_16u* hash_size);

extern gf_status gf_file_info_get_inode(
  const gf_file_info* info, gf_16u* inode);

extern gf_status gf_file_info_get_mode(
  const gf_file_info* info, gf_16u* mode);

extern gf_status gf_file_info_get_link_count(
  const gf_file_info* info, gf_16s* link_count);

extern gf_status gf_file_info_get_uid(
  const gf_file_info* info, gf_16s* uid);

extern gf_status gf_file_info_get_gid(
  const gf_file_info* info, gf_16s* gid);

extern gf_status gf_file_info_get_device(
  const gf_file_info* info, gf_32u* device);

extern gf_status gf_file_info_get_rdevice(
  const gf_file_info* info, gf_32u* rdevice);

extern gf_status gf_file_info_get_file_size(
  const gf_file_info* info, gf_64u* file_size);

extern gf_status gf_file_info_get_access_time(
  const gf_file_info* info, gf_64u* access_time);

extern gf_status gf_file_info_get_modify_time(
  const gf_file_info* info, gf_64u* modify_time);

extern gf_status gf_file_info_get_create_time(
  const gf_file_info* info, gf_64u* create_time);

/* -------------------------------------------------------------------------- */

extern gf_status gf_file_info_set_file_name(
  gf_file_info* info, const gf_char* file_name);

extern gf_status gf_file_info_set_full_path(
  gf_file_info* info, const gf_char* full_path);

extern gf_status gf_file_info_set_hash(
  gf_file_info* info, gf_size_t size, gf_8u* hash);

extern gf_status gf_file_info_set_hash_string(
  gf_file_info* info, gf_size_t size, gf_8u* str);

extern gf_status gf_file_info_set_user_data(
  gf_file_info* info, gf_any user_data);

extern gf_status gf_file_info_set_user_flag(
  gf_file_info* info, gf_32u user_flag);

extern gf_status gf_file_info_set_hash_size(
  gf_file_info* info, gf_16u hash_size);

extern gf_status gf_file_info_set_inode(gf_file_info* info, gf_16u inode);

extern gf_status gf_file_info_set_mode(gf_file_info* info, gf_16u mode);

extern gf_status gf_file_info_set_link_count(
  gf_file_info* info, gf_16s link_count);

extern gf_status gf_file_info_set_uid(gf_file_info* info, gf_16s uid);

extern gf_status gf_file_info_set_gid(gf_file_info* info, gf_16s gid);

extern gf_status gf_file_info_set_device(gf_file_info* info, gf_32u device);

extern gf_status gf_file_info_set_rdevice(gf_file_info* info, gf_32u rdevice);

extern gf_status gf_file_info_set_file_size(
  gf_file_info* info, gf_64u file_size);

extern gf_status gf_file_info_set_access_time(
  gf_file_info* info, gf_64u access_time);

extern gf_status gf_file_info_set_modify_time(
  gf_file_info* info, gf_64u modify_time);

extern gf_status gf_file_info_set_create_time(
  gf_file_info* info, gf_64u create_time);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_FILE_INFO_H */
