/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_error.h
** @brief Error handling module.
*/
#ifndef LIBGF_GF_ERROR_H
#define LIBGF_GF_ERROR_H

#pragma once

#include <libgf/config.h>

#include <assert.h>
#include <stdarg.h>
#include <signal.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_log.h>

/*!
** @brief Make error report.
**
**
**
** @param [in] code Status code
** @param [in] file The file name of the caller
** @param [in] line The line number of the caller
** @param [in] fmt  Message string
** @param [in] ...  The format arguments
*/

extern void gf_safe_error(
  gf_status code, const char* file, int line, const char* fmt, ...);

#define gf_throw(code)       \
  do {                       \
    gf_status c_ = (code);   \
    if (c_ != GF_SUCCESS) {  \
      return c_;             \
    }                        \
  } while (0)

/*!
** @brief Raise error
**
**
**
*/

#if defined(GF_USE_SAFE_ERROR_)
# define gf_raise(code, mess, ...)                                 \
  do {                                                             \
    gf_safe_error(code, __FILE__, __LINE__, mess, ##__VA_ARGS__);  \
    return (code);                                                 \
  } while (0)
#else   /* GF_PRIMITIVE_LOGGER_ */
# define gf_raise(code, mess, ...)          \
  do {                                      \
    gf_error(mess, ##__VA_ARGS__);          \
    return (code);                          \
  } while (0)
#endif  /* GF_PRIMITIVE_LOGGER_ */

/*!
**
**
**
**
*/

#define gf_validate(cond)                          \
  do {                                             \
    gf_bool c_ = GF_FALSE;                         \
    c_ = (cond);                                   \
    if (!(c_)) {                                   \
      gf_raise(GF_E_PARAM, "Invalid parameter.");  \
    }                                              \
  } while(0)



#endif  /* LIBGF_GF_ERROR_H */
