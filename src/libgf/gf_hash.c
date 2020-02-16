/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_hash.c
** @brief Hash functions
*/
#include <windows.h>
#include <wincrypt.h>

#include <libgf/gf_local.h>
#include <libgf/gf_memory.h>
#include <libgf/gf_hash.h>


struct gf_hash {
  gf_hash_type type;
  HCRYPTPROV hProv;
  HCRYPTHASH hHash;
};


static gf_status
hash_init(gf_hash* hash) {
  gf_validate(hash);

  hash->type = GF_HASH_TYPE_UNKNOWN;
  hash->hProv = 0;
  hash->hHash = 0;
  
  return GF_SUCCESS;
}

static gf_status
hash_translate_algorithm_id(ALG_ID* id, gf_hash_type type) {
  gf_validate(id);
  
  switch (type) {
  case GF_HASH_TYPE_SHA_256:
    *id = CALG_SHA_256;
    break;
  case GF_HASH_TYPE_SHA_512:
    *id = CALG_SHA_512;
    break;
  case GF_HASH_TYPE_UNKNOWN:
  default:
    gf_raise(GF_E_PARAM, "Invalid hash type.");
  }

  return GF_SUCCESS;
}

static gf_status
hash_setup(gf_hash* hash, gf_hash_type type) {
  BOOL ret = FALSE;
  HCRYPTPROV hProv = 0;
  HCRYPTHASH hHash = 0;
  ALG_ID id = 0;

  static LPCSTR container_name = "GrayfishContainer";
  
  gf_validate(hash);

  _(hash_translate_algorithm_id(&id, type));
  
  ret = CryptAcquireContext(&hProv, container_name, NULL, PROV_RSA_FULL, 0);
  if (!ret) {
    gf_raise(GF_E_INTERNAL, "Failed to aquire the cryptography context.");
  }
  ret = CryptCreateHash(hProv, id, 0, 0, &hHash);
  if (!ret) {
    CryptReleaseContext(hProv, 0);
    gf_raise(GF_E_INTERNAL, "Failed to create the hash handle.");
  }
  
  hash->hProv = hProv;
  hash->hHash = hHash;

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
  rc = hash_setup(tmp, type);
  if (rc != GF_SUCCESS) {
    gf_hash_free(tmp);
    gf_throw(rc);
  }
  
  *hash = tmp;
              
  return GF_SUCCESS;
}

void
gf_hash_free(gf_hash* hash) {
  if (hash) {
    if (hash->hHash) {
      CryptDestroyHash(hash->hHash);
      hash->hHash = 0;
    }
    if (hash->hProv) {
      CryptReleaseContext(hash->hProv, 0);
      hash->hProv = 0;
    }
    gf_free(hash);
  }
}
