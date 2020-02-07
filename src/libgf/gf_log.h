/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_log.h
** @brief Logger module.
*/
#ifndef LIBGF_GF_LOG_H
#define LIBGF_GF_LOG_H

#pragma once

#include <stdarg.h>
#include <stdint.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>
#include <libgf/gf_stream.h>

typedef struct gf_log gf_log;

/*!
** @brief The contants for logging level.
*/

enum gf_log_level {
  GF_LOG_TRACE = 0,  ///<
  GF_LOG_DEBUG = 1,  ///<
  GF_LOG_INFO  = 2,  ///<
  GF_LOG_WARN  = 3,  ///<
  GF_LOG_ERROR = 4,  ///<
  GF_LOG_FATAL = 5,  ///<
};

typedef enum gf_log_level gf_log_level;

/*!
** @brief Initialize the logging handle
*/

extern void gf_log_init(void);

/*!
** @brief Clear the logging handle
*/

extern void gf_log_clean(void);

/*!
** @brief Set the logging level
**
** @param [in] level The logging level
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_log_set_level(gf_log_level level);

/*!
** @brief Set the logging level by a string.
**
**   - <code>trace</code> 
**   - <code>debug</code> 
**   - <code>info</code> 
**   - <code>warn</code> 
**   - <code>error</code> 
**   - <code>fatal</code> 
**
** @param [in] str The string of the logging level
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_log_set_level_string(const char* str);

/*!
**
**
**
*/

extern gf_status gf_log_add_stream(gf_write_stream* stream);

/*!
** @brief Write the log.
**
** @param [in] level The logging level
** @param [in] file  The file name, in which logging event has occured
** @param [in] line  The line number, in which logging event has occured
** @param [in] fmt   The message format string 
** @param [in] ...   The message format arguments
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_log_write(
  gf_log_level level, const char* file, int line, const char* fmt, ...);

/*!
** @defgroup gf_log_macro The utility macros
*/
///@{
#define gf_trace(msg, ...) \
  gf_log_write(GF_LOG_TRACE, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define gf_debug(msg, ...) \
  gf_log_write(GF_LOG_DEBUG, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define gf_info(msg, ...)  \
  gf_log_write(GF_LOG_INFO,  __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define gf_warn(msg, ...)  \
  gf_log_write(GF_LOG_WARN,  __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define gf_error(msg, ...) \
  gf_log_write(GF_LOG_ERROR, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define gf_fatal(msg, ...) \
  gf_log_write(GF_LOG_FATAL, __FILE__, __LINE__, msg, ##__VA_ARGS__)
///@}

extern gf_status gf_msg(const char* fmt, ...);


#endif  /* LIBGF_GF_LOG_H */
