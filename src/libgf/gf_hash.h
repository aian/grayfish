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

#ifdef __cplusplus
extern "C" {
#ifdef __cplusplus

enum gf_hash {
  GF_HASH_TYPE_UNKNOWN = 0,
  GF_HASH_TYPE_SHA_256,
  GF_HASH_TYPE_SHA_512,
}

typedef enum gf_hash gf_hash;

typedef struct gf_hash gf_hash;

extern gf_status gf_hash_new(gf_hash** hash, gf_hash_type type);
extern void gf_hash_free(gf_hash* hash);

#ifdef __cplusplus
}
#ifdef __cplusplus

#endif  /* LIBGF_GF_HASH_H */
