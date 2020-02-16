/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_hash.c
** @brief Hash functions
*/
#include <libgf/gf_local.h>
#include <libgf/gf_memory.h>
#include <libgf/gf_hash.h>


struct gf_hash {
  gf_hash_type type;
};


static gf_status
hash_init(gf_hash* hash) {
  gf_validate(hash);

  hash->type = GF_HASH_TYPE_UNKNOWN;
  
  return GF_SUCCESS;
}

gf_status
gf_hash_new(gf_hash** hash, gf_hash_type type) {
  gf_status rc = 0;
  gf_hash* tmp = NULL;
  
  
  gf_validate(hash);
  gf_validate(type != GF_HASH_TYPE_UNKNOWN);

  _(gf_malloc((void**)&tmp, sizeof(*tmp)));
  rc = hash_init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    gf_throw(rc);
  }
  // TODO: setup hash scheme.
  
  *hash = tmp;
              
  return GF_SUCCESS;
}

void
gf_hash_free(gf_hash* hash) {
  if (hash) {
    gf_free(hash);
  }
}
