/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_memory.h
** @brief Memory management.
*/
#ifndef LIBGF_GF_MEMORY_H
#define LIBGF_GF_MEMORY_H

#pragma once

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>

#ifdef __cplusplus
extern "C" {
#endif

extern gf_status gf_malloc(gf_ptr* ptr, gf_size_t size);

extern gf_status gf_realloc(gf_ptr* ptr, gf_size_t size);

extern void gf_free(gf_ptr ptr);

extern gf_status gf_memset(gf_ptr buf, gf_int ch, gf_size_t n);

extern gf_status gf_bzero(gf_ptr buf, gf_size_t n);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_MEMORY_H */
