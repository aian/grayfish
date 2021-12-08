/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_xslt.h
** @brief XSLT processor.
*/
#ifndef LIBGF_GF_XSLT_H
#define LIBGF_GF_XSLT_H

#pragma once

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>
#include <libgf/gf_path.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gf_xslt gf_xslt;

/*!
** @param [out] xslt A pointer to the pointer, which points to the new context
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_xslt_new(gf_xslt** xslt);

/*!
**
*/

extern void gf_xslt_free(gf_xslt* xslt);

/*!
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_xslt_reset(gf_xslt* xslt);

/*!
**
** @param [in, out] xslt The xslt context obejct
** @param [in]      path The template path
**
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_xslt_read_template(gf_xslt* xslt, const gf_path* path);

/*!
** @brief Do the XSLT processing.
**
** @param [in, out] xslt File xslt context
** @param [in]      path XML file path, from which is converted.
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_xslt_process(gf_xslt* xslt, const gf_path* path);

/*!
** @brief Write a result file.
**
** @param [in, out] xslt File xslt context
** @param [in]      path Output file path
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_xslt_write_file(gf_xslt* xslt, const gf_path* path);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_XSLT_H */
