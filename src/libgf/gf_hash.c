/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_hash.c
** @brief Hash functions
*/
#include <openssl/sha.h>

#include <libgf/gf_memory.h>
#include <libgf/gf_hash.h>

#include "gf_local.h"

#define GF_HASH_BUFSIZE_FILE 4096

#define OPENSSL_RAISE(ret)                                \
  if (ret == 0) {                                         \
    gf_raise(GF_E_API, "Failed to calcurate file hash."); \
  }


gf_status
gf_hash_file(gf_8u* hash, gf_size_t size, gf_path* path) {
  int ret = 0;
  SHA512_CTX ctxt = { 0 };
  FILE* fp = NULL;
  gf_8u buffer[GF_HASH_BUFSIZE_FILE] = { 0 };
  size_t read_bytes = 0;

  static const size_t s_bufsize = GF_HASH_BUFSIZE_FILE;
  
  gf_validate(hash);
  gf_validate(size < GF_HASH_BUFSIZE_SHA512);
  gf_validate(!gf_path_is_empty(path));
  
  ret = SHA512_Init(&ctxt);
  OPENSSL_RAISE(ret);
  
  fp = fopen(gf_path_get_string(path), "rb");
  if (!fp) {
    gf_raise(GF_E_OPEN, "Failed to open file. (%s)", gf_path_get_string(path));
  }
  while ((read_bytes = fread(buffer, sizeof(buffer[0]), s_bufsize, fp)) > 0) {
    ret = SHA512_Update(&ctxt, buffer, read_bytes);
    OPENSSL_RAISE(ret);
  }

  ret = SHA512_Final(hash, &ctxt);
  OPENSSL_RAISE(ret);

  return GF_SUCCESS;
}
