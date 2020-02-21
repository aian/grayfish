/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_convert.h
** @brief Abstract API to convert files.
*/
#ifndef LIBGF_GF_CONVERT_H
#define LIBGF_GF_CONVERT_H

#pragma once

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>

#ifdef __cplusplus
extern "C" {
#endif

enum gf_convert_type {
  GF_CONVERT_TYPE_UNKNOWN = 0,
  GF_CONVERT_TYPE_XSLT,
  GF_CONVERT_TYPE_MARKDOWN,
};

typedef enum gf_convert_type gf_convert_type;


typedef struct gf_convert_ctxt gf_convert_ctxt;

/*!
** @param [out] ctxt A pointer to the pointer, which points to the new context
** @param [in]  type A destination file type
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_convert_ctxt_new(
  gf_convert_ctxt** ctxt, gf_convert_type type);

/*!
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern void gf_convert_ctxt_free(gf_convert_ctxt* ctxt);

/*!
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_convert_ctxt_reset(gf_convert_ctxt* ctxt);

/*!
**
** @param [in, out] ctxt The convert context obejct
** @param [in]      path The template path
**
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_convert_ctxt_read_template(
  gf_convert_ctxt* ctxt, const char* path);

/*!
** @brief Convert the file according to the context scheme.
**
** @param [in, out] ctxt File convert context
** @param [in]      dst  Destination file path
** @param [in]      src  Source file path
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_convert_file(
  gf_convert_ctxt* ctxt, const char* dst, const char* src);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_CONVERT_H */
