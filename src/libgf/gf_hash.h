/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_hash.h
** @brief Hash functions
*/
#ifndef LIBGF_GF_HASH_H
#define LIBGF_GF_HASH_H

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>

#include <libgf/gf_path.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GF_HASH_BUFSIZE_SHA512 64

extern gf_status gf_hash_file(gf_8u* buffer, gf_size_t size, gf_path* path);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_HASH_H */
