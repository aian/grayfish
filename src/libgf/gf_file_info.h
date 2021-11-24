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
** @param [in]  path A path of the file from which you want to get information.
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/
extern gf_status gf_file_info_new(gf_file_info** info, const gf_path* path);

/*!
** @brief Discards the gf_file_info object.
**
** @param [in, out] info 
*/
extern void gf_file_info_free(gf_file_info* info);

extern gf_bool gf_file_info_is_file(const gf_file_info* info);
extern gf_bool gf_file_info_is_directory(const gf_file_info* info);

extern gf_size_t gf_file_info_count_children(const gf_file_info* info);

extern gf_status gf_file_info_add_child(
  gf_file_info* info, gf_file_info* child);
extern gf_status gf_file_info_get_child(
  const gf_file_info* info, gf_size_t index, gf_file_info** child);

extern gf_status gf_file_info_get_file_name(
  const gf_file_info* info, const gf_char** file_name);
extern gf_status gf_file_info_get_full_path(
  const gf_file_info* info, const gf_char** full_path);

extern gf_status gf_file_info_scan(gf_file_info** info, const gf_path* path);


#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_FILE_INFO_H */
