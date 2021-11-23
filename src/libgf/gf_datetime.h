/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_datetime.h
** @brief Datetime management
*/
#ifndef LIBGF_GF_DATETIME_H
#define LIBGF_GF_DATETIME_H

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>

#include <libgf/gf_string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef gf_64u gf_datetime;

extern gf_status gf_date_parse(const gf_char* str, gf_datetime* datetime);
extern gf_status gf_date_make_string(gf_string* str, gf_datetime datetime);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_DATETIME_H */
