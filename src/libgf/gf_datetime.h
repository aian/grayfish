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

extern gf_status gf_datetime_get_current_time(gf_datetime* datetime);

/*!
** @brief Parse a datetime string as a extended ISO 8061 format.
**
** An acceptable format string is "YYYY-mm-DD HH:MM:SS" only. This format is
** strictly checked. Any exceptions are NOT accepted.
**
** @param [in]  str A datetime string to be parsed
** @param [out] datetime A result datetime value
**
** @return GF_SUCCESS if success, GF_E_* otherwise.
*/

extern gf_status gf_datetime_parse_iso8061_string(
  const gf_char* str, gf_datetime* datetime);

extern gf_status gf_datetime_make_string(
  gf_string* str, const gf_char* fmt, gf_datetime datetime);

/*!
** @brief Convert a datetime value into a string as a extended ISO 8061 format.
**
** @param [out] str A result datetime string
** @param [in]  datetime A datetime value to be converted
**
** @return GF_SUCCESS if success, GF_E_* otherwise.
*/

extern gf_status gf_datetime_make_iso8061_string(
  gf_string* str, gf_datetime datetime);

extern gf_status gf_datetime_make_current_string(
  gf_string* str, const gf_char* fmt, gf_datetime datetime);

/*!
** @brief Convert a datetime value into a string as a YYMMDDhhmmss format.
**
** @param [out] str A result datetime string
**
** @return GF_SUCCESS if success, GF_E_* otherwise.
*/

extern gf_status gf_datetime_make_current_digit_string(gf_string* str);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_DATETIME_H */
