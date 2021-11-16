/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_date.h
** @brief Datetime management
*/
#ifndef LIBGF_GF_DATE_H
#define LIBGF_GF_DATE_H

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gf_date gf_date;

extern gf_status gf_date_new(gf_date** date);
extern void gf_date_free(gf_date* date);
extern gf_status gf_date_copy(gf_date* dst, const gf_date* src);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_DATE_H */
