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
#endif

enum gf_hash_type {
  GF_HASH_TYPE_UNKNOWN = 0,
  GF_HASH_TYPE_SHA_256,
  GF_HASH_TYPE_SHA_512,
};

typedef enum gf_hash_type gf_hash_type;

typedef struct gf_hash gf_hash;

/*!
** @brief Create a hash context handle
**
** @param hash [out] The pointer to the new context handle
** @param type [in]  The hashing algorithm type
**
*/

extern gf_status gf_hash_new(gf_hash** hash, gf_hash_type type);

/*!
** @brief Destroy the hash context handle
**
** @param hash [in] The pointer to the context handle to destroy
**
*/

extern void gf_hash_free(gf_hash* hash);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_HASH_H */
