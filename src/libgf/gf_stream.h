/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_stream.h
** @brief File stream module.
*/
#ifndef LIBGF_GF_STREAM_H
#define LIBGF_GF_STREAM_H

#pragma once

#include <libgf/config.h>

#include <stdio.h>
#include <stdarg.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>
#include <libgf/gf_stream.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef gf_status (*gf_open_fn)(gf_ptr* ptr, const char* dst);
typedef void (*gf_close_fn)(gf_ptr* ptr);
typedef int (*gf_write_fn)(gf_ptr ptr, const char* fmt, va_list args);

typedef struct gf_write_stream gf_write_stream;

extern gf_status gf_stream_new(
  gf_write_stream **stream, gf_open_fn open, gf_close_fn close, gf_write_fn write);

extern void gf_stream_free(gf_write_stream* stream);

extern gf_status gf_stream_open(gf_write_stream* stream, const char* dst);
extern void gf_stream_close(gf_write_stream *stream);
extern gf_status gf_stream_write(gf_write_stream *stream, const char* fmt, ...);

extern gf_bool gf_stream_is_open(const gf_write_stream* stream);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_STREAM_H */
