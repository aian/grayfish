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

#ifdef __cplusplus
extern {
#endif

typedef struct gf_file_info gf_file_info;

extern gf_status gf_file_info_new(gf_file_info** info, const gf_path* path);
extern void gf_file_info_free(gf_file_info* info);

extern gf_bool gf_file_info_is_file(const gf_file_info* info);
extern gf_bool gf_file_info_is_directory(const gf_file_info* info);

extern gf_size_t gf_file_info_count_children(const gf_file_info* info);

extern gf_status gf_file_info_add_child(
  gf_file_info* info, gf_file_info* child);
extern gf_status gf_file_info_get_child(
  const gf_file_info* info, gf_size_t index, gf_file_info** child);

extern gf_status gf_file_info_scan(gf_file_info** info, const gf_path* path);


#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_FILE_INFO_H */
