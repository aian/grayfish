/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_error.c
** @brief Error handling module.
*/
#include <stdio.h>
#include <windows.h>

#define GF_USE_SAFE_ERROR_ 1

#include <libgf/gf_error.h>

#define ERROR_MESSAGE_LENGTH 1024

void
gf_safe_error(
  gf_status code, const char* file, int line, const char* fmt, ...) {

  static const char nullstr[] = "";
  static char buf[ERROR_MESSAGE_LENGTH + 1] = { 0 };

  const char* str = NULL;
  SYSTEMTIME tm = { 0 };
  va_list arg = { 0 };

  if (fmt && *fmt) {
    va_start(arg, fmt);
    vsprintf_s(buf, ERROR_MESSAGE_LENGTH, fmt, arg);
    va_end(arg);
    str = buf;
  } else {
    str = nullstr;
  }

  /* Current time */
  GetLocalTime(&tm);
  
#if defined(GF_DEBUG_)
  fprintf(stderr, "%s:%d: [%04d/%02d/%02d %02d:%02d:%02d.%03d] error: %s\n",
          file, line,
          tm.wYear, tm.wMonth, tm.wDay, tm.wHour,
          tm.wMinute, tm.wSecond, tm.wMilliseconds,
          str);
#else
  (void)file;
  (void)line;
  fprintf(stderr, "[%04d/%02d/%02d %02d:%02d:%02d.%03d] error: %s\n",
          tm.wYear, tm.wMonth, tm.wDay, tm.wHour,
          tm.wMinute, tm.wSecond, tm.wMilliseconds,
          str);
#endif
  fprintf(stderr, "Return Code: 0x%04X\n", code);
}
