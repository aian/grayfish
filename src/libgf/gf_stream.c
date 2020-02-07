/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_stream.c
** @brief File stream module.
*/
#include <libgf/gf_memory.h>
#include <libgf/gf_stream.h>

#include <libgf/gf_local.h>

struct gf_write_stream {
  FILE* fp;
  gf_open_fn open;
  gf_close_fn close;
  gf_write_fn write;
};

static gf_status
write_stream_init(gf_write_stream* stream) {
  gf_validate(stream);

  stream->fp = NULL;
  stream->open = NULL;
  stream->close = NULL;
  stream->write = NULL;

  return GF_SUCCESS;
}

gf_status
gf_stream_new(
  gf_write_stream **stream, gf_open_fn open, gf_close_fn close, gf_write_fn write) {
  gf_status rc = 0;
  gf_write_stream *tmp = NULL;
  
  gf_validate(stream);

  _(gf_malloc((gf_ptr*)&tmp, sizeof(*tmp)));

  rc = write_stream_init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    gf_raise(GF_E_OPEN, "Failed to open stream.");
  }
  tmp->fp = NULL;
  tmp->open = open;
  tmp->close = close;
  tmp->write = write;
  
  *stream = tmp;

  return GF_SUCCESS;
}

void
gf_stream_free(gf_write_stream* stream) {
  if (stream) {
    if (gf_stream_is_open(stream)) {
      gf_stream_close(stream);
    }
    stream->open = NULL;
    stream->close = NULL;
    stream->write = NULL;

    gf_free(stream);
  }
}

gf_status
gf_stream_open(gf_write_stream* stream, const char* dst) {
  gf_validate(stream);

  if (!stream->open) {
    gf_raise(GF_E_OPEN, "Failed to open stream.");
  }
  _(stream->open((gf_ptr*)&stream->fp, dst));

  return  GF_SUCCESS;
}

void
gf_stream_close(gf_write_stream *stream) {
  if (stream->close) {
    stream->close((gf_ptr*)&stream->fp);
  }
}

gf_status
gf_stream_write(gf_write_stream *stream, const char* fmt, ...) {
  va_list args = { 0 };
  int write_size = 0;

  gf_validate(stream);

  if (!stream->write) {
    gf_raise(GF_E_WRITE, "Failed to write to the stream.");
  }
  if (gf_stream_is_open(stream)) {
    va_start(args, fmt);
    write_size = stream->write(stream->fp, fmt, args);
    va_end(args);
    if (write_size < 0) {
      gf_raise(GF_E_WRITE, "Failed to write to the stream.");
    }
  }

  return GF_SUCCESS;
}

gf_bool
gf_stream_is_open(const gf_write_stream* stream) {
  return stream && stream->fp ? GF_TRUE : GF_FALSE;
}
