/*-
*  This file is part of Grayfish project. For license details, see the file
*  'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_log.c
** @brief Logger module.
*/
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <windows.h>

#define GF_USE_SAFE_ERROR_ 1

#include <libgf/gf_countof.h>
#include <libgf/gf_memory.h>
#include <libgf/gf_string.h>
#include <libgf/gf_log.h>

#include "gf_local.h"

struct log_level_info {
  gf_log_level level;
  char*        label;
  char*        prefix;
};

typedef struct log_level_info log_level_info;

static const log_level_info log_level_info_[] = {
  { .level = GF_LOG_TRACE, .label = "trace", .prefix="trace: "   },
  { .level = GF_LOG_DEBUG, .label = "debug", .prefix="debug: "   },
  { .level = GF_LOG_INFO,  .label = "info",  .prefix=""          },
  { .level = GF_LOG_WARN,  .label = "warn",  .prefix="warning: " },
  { .level = GF_LOG_ERROR, .label = "error", .prefix="error: "   },
  { .level = GF_LOG_FATAL, .label = "fatal", .prefix="fatal: "   },
};

/*!
** @brief Logger structure.
*/

struct gf_log {
  gf_log_level      level;   ///< Log level
  gf_write_stream** stream;  ///< The set of the file pointers for logging
  gf_size_t         used;
  gf_size_t         size;
};

static gf_log logger_ = {
  .level  = GF_LOG_INFO,
  .stream = NULL,
  .used   = 0,
  .size   = 0,
};

#define LOG_WRITE_STREAM_SIZE 16

static gf_status
log_prepare(void) {
  gf_write_stream** stream = NULL;

  _(gf_malloc((gf_ptr*)&stream, sizeof(*stream) * LOG_WRITE_STREAM_SIZE));

  /* Initialize */
  for (gf_size_t i = 0; i < LOG_WRITE_STREAM_SIZE; i++) {
    stream[i] = NULL;
  }
  /* Assign */
  if (logger_.stream) {
    gf_free(logger_.stream);
  }
  logger_.stream = stream;
  logger_.used = 0;
  logger_.size = LOG_WRITE_STREAM_SIZE;

  return GF_SUCCESS;
}

void
gf_log_init(void) {
  gf_log_clean();

  logger_.level  = GF_LOG_INFO;
  logger_.stream = NULL;
  logger_.used   = 0;
  logger_.size   = 0;

  if (log_prepare()) {
    // TODO: error message
    gf_log_clean();
  }
}

void
gf_log_clean(void) {
  logger_.level = GF_LOG_INFO;
  if (logger_.stream) {
    for (gf_size_t i = 0; i < logger_.used; i++) {
      if (logger_.stream[i]) {
        gf_stream_close(logger_.stream[i]);
        logger_.stream[i] = NULL;
      }
    }
    gf_free(logger_.stream);
    logger_.stream = NULL;
  }
  logger_.used = 0;
  logger_.size = 0;
}

gf_status
gf_log_set_level(gf_log_level level) {
  switch (level) {
  case GF_LOG_TRACE:
  case GF_LOG_DEBUG:
  case GF_LOG_INFO:
  case GF_LOG_WARN:
  case GF_LOG_ERROR:
  case GF_LOG_FATAL:
    logger_.level =level;
    break;
  default:
    gf_raise(GF_E_PARAM, "Invalid log level.");
    break;
  }
  return GF_SUCCESS;
}

gf_status
gf_log_set_level_string(const char* str) {
  static const gf_size_t cnt = gf_countof(log_level_info_);
  
  gf_validate(str);

  for (gf_size_t i = 0; i < cnt; i++) {
    if (!stricmp(log_level_info_[i].label, str)) {
      _(gf_log_set_level(log_level_info_[i].level));
      return GF_SUCCESS;
    }
  }
  /* No log level has been found */
  gf_raise(GF_E_PARAM, "Invalid log level string.");
}

gf_status
gf_log_add_stream(gf_write_stream* stream) {
  gf_validate(stream);
  
  if (logger_.used >= logger_.size) {
    gf_size_t size = logger_.size * 2;

    _(gf_realloc((gf_ptr*)&logger_.stream, sizeof(stream) * size));
    logger_.size = size;
    /* Initialize the realloced space */
    for (gf_size_t i = logger_.used; i < logger_.size; i++) {
      logger_.stream[i] = NULL;
    }
  }

  logger_.stream[logger_.used] = stream;
  logger_.used += 1;

  return GF_SUCCESS;
}

static gf_status 
log_get_log_level_info(gf_log_level level, const log_level_info** info) {
  static const gf_size_t cnt = gf_countof(log_level_info_);

  gf_validate(info);

  for (gf_size_t i = 0; i < cnt; i++) {
    if (log_level_info_[i].level == level) {
      *info = &log_level_info_[i];
      return GF_SUCCESS;
    }
  }
  /* No log level has been found */
  gf_raise(GF_E_PARAM, "Invalid log level.");
}

static gf_status
log_write(
  const log_level_info* info, const char* file, int line, const char* msg) {

  SYSTEMTIME tm = { 0 };

  gf_validate(info);
  gf_validate(file);
  gf_validate(msg);

  /* Current time */
  GetLocalTime(&tm);
  
  for (gf_size_t i = 0; i < logger_.used; i++) {
#if defined(GF_DETAIL_LOG_)
# if defined(GF_DEBUG_)
    /* detailed debug log */
    gf_stream_write(
      logger_.stream[i], "%s:%d: [%04d/%02d/%02d %02d:%02d:%02d.%03d] %s%s\n",
      file, line,
      tm.wYear, tm.wMonth, tm.wDay, tm.wHour,
      tm.wMinute, tm.wSecond, tm.wMilliseconds,
      info->prefix, msg);
# else
    /* detailed log */
    (void)file;
    (void)line;
    gf_stream_write(
      logger_.stream[i], "[%04d/%02d/%02d %02d:%02d:%02d.%03d] %s%s\n",
      tm.wYear, tm.wMonth, tm.wDay, tm.wHour,
      tm.wMinute, tm.wSecond, tm.wMilliseconds,
      info->prefix, msg);
# endif
#else
    /* normal log */
    (void)file;
    (void)line;
    (void)tm;

    gf_stream_write(logger_.stream[i], "%s%s\n", info->prefix, msg);
#endif
  }
  
  return GF_SUCCESS;
}

static gf_status
log_build_message(char** msg, const char* fmt, va_list args) {
  char* tmp = NULL;
  int len = 0;

  gf_validate(msg);
  gf_validate(fmt);

  /* Count characters of the message */
  len = _vscprintf(fmt, args) + 1;
  /* Write message */
  _(gf_malloc((gf_ptr*)&tmp, len));
  vsprintf_s(tmp, len, fmt, args);

  *msg = tmp;
  
  return GF_SUCCESS;
}

gf_status
gf_log_write(
  gf_log_level level, const char* file, int line, const char* fmt, ...) {

  gf_status rc = 0;
  char* msg = NULL;
  const log_level_info* info = NULL;
  va_list args;

  gf_validate(!gf_strnull(file));
  gf_validate(fmt);

  if (level < logger_.level) {
    return GF_SUCCESS;
  }
  /* Get the log info entry */
  _(log_get_log_level_info(level, &info));
  
  /* Make a message string */
  va_start(args, fmt);
  rc = log_build_message(&msg, fmt, args);
  va_end(args);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  /* Print out */
  rc = log_write(info, file, line, msg);
  gf_free(msg);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  
  return GF_SUCCESS;
}

gf_status
gf_msg(const char* fmt, ...) {
  gf_status rc = 0;
  char* msg = NULL;
  va_list args;

  gf_validate(fmt);
  
  /* Make a message string */
  va_start(args, fmt);
  rc = log_build_message(&msg, fmt, args);
  va_end(args);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  /* Print out */
  for (gf_size_t i = 0; i < logger_.used; i++) {
    gf_stream_write(logger_.stream[i], "%s\n", msg);
  }
  gf_free(msg);

  return GF_SUCCESS;
}
