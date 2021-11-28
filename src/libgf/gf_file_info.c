/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_file_info.h
** @brief Operating file information.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <libgf/gf_memory.h>
#include <libgf/gf_hash.h>
#include <libgf/gf_array.h>
#include <libgf/gf_file_info.h>

#include "gf_local.h"

struct gf_file_info {
  gf_path*  file_name;                    ///< file name
  gf_path*  full_path;                    ///< full path name
  gf_8u     hash[GF_HASH_BUFSIZE_SHA512]; ///< SHA-512 hash
  gf_any    user_data;                    ///< user defined data
  gf_32u    user_flag;                    ///< user defined flag
  gf_16u    hash_size;                    ///< hash buffer size (in byte)
  gf_16u    inode;                        ///< stat64::st_ino
  gf_16u    mode;                         ///< stat64::st_mode
  gf_16s    link_count;                   ///< stat64::st_nlink
  gf_16s    uid;                          ///< stat64::st_uid
  gf_16s    gid;                          ///< stat64::st_gid
  gf_32u    device;                       ///< stat64::st_dev 
  gf_32u    rdevice;                      ///< stat64::st_rdev
  gf_64u    file_size;                    ///< stat64::st_size
  gf_64u    access_time;                  ///< stat64::st_atime
  gf_64u    modify_time;                  ///< stat64::st_mtime
  gf_64u    create_time;                  ///< stat64::st_ctime
  gf_array* children;
};

static gf_status
file_info_init(gf_file_info* info) {
  gf_validate(info);

  info->file_name = NULL;
  info->full_path = NULL;
  info->inode = 0;
  info->mode = 0;
  info->link_count= 0;
  info->uid = 0;
  info->gid = 0;
  info->device = 0;
  info->rdevice = 0;
  info->file_size = 0;
  info->access_time = 0;
  info->modify_time = 0;
  info->create_time = 0;
  info->children = NULL;

  info->user_data.data = 0;
  info->user_flag = 0;
  
  info->hash_size = GF_HASH_BUFSIZE_SHA512;
  _(gf_bzero(info->hash, info->hash_size));
  
  return GF_SUCCESS;
}

static gf_status
file_info_set_path(gf_file_info* info, const gf_path* path) {
  gf_status rc = 0;
  gf_path* full_path = NULL;
  gf_path* file_name = NULL;
  
  gf_validate(info);
  gf_validate(path);

  /*
  ** set full path name
  */
  _(gf_path_clone(&full_path, path));

  rc = gf_path_absolute_path(full_path);
  if (rc != GF_SUCCESS) {
    gf_path_free(full_path);
    gf_throw(rc);
  }
  info->full_path = full_path;
  /*
  ** check if the path exist
  */
  if (!gf_path_file_exists(full_path)) {
    gf_raise(
      GF_E_PARAM, "File does not exist. (%s)", gf_path_get_string(full_path));
  }
  /*
  ** set file name
  */
  _(gf_path_clone(&file_name, full_path));

  rc = gf_path_file_name(file_name);
  if (rc != GF_SUCCESS) {
    gf_path_free(file_name);
    gf_throw(rc);
  }
  info->file_name = file_name;
  
  return GF_SUCCESS;
}

static gf_status
file_info_set_stat(gf_file_info* info) {
  int ret = 0;
  struct stat64 st = { 0 };

  gf_validate(info);

  ret = stat64(gf_path_get_string(info->full_path), &st);
  if (ret != 0) {
    gf_raise(GF_E_API, "Could not get a file information.");
  }
  info->inode       = st.st_ino;
  info->mode        = st.st_mode;
  info->link_count  = st.st_nlink;
  info->uid         = st.st_uid;
  info->gid         = st.st_gid;
  info->device      = st.st_dev;
  info->rdevice     = st.st_rdev;
  info->file_size   = st.st_size;
  info->access_time = st.st_atime;
  info->modify_time = st.st_mtime;
  info->create_time = st.st_ctime;

  return GF_SUCCESS;
}

static gf_status
file_info_set_hash(gf_file_info* info) {
  gf_validate(info);

  _(gf_hash_file(info->hash, GF_HASH_BUFSIZE_SHA512, info->full_path));
  
  return GF_SUCCESS;
}

static void
file_info_free(gf_any* any) {
  if (any) {
    if (any->ptr) {
      gf_file_info_free((gf_file_info*)any->ptr);
    }
  }
}

static gf_status
file_info_prepare_children(gf_file_info* info) {
  gf_array* children = 0;
  
  gf_validate(info);

  _(gf_array_new(&children));
  info->children = children;
  _(gf_array_set_free_fn(info->children, file_info_free));
  
  return GF_SUCCESS;
}

gf_status
gf_file_info_new(gf_file_info** info, const gf_path* path) {
  gf_status rc = 0;
  gf_file_info* tmp = NULL;
  
  gf_validate(info);
  gf_validate(path);

  _(gf_malloc((gf_ptr*)&tmp, sizeof(*tmp)));
  rc = file_info_init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    gf_throw(rc);
  }
  rc = file_info_set_path(tmp, path);
  if (rc != GF_SUCCESS) {
    gf_file_info_free(tmp);
    gf_throw(rc);
  }
  rc = file_info_set_stat(tmp);
  if (rc != GF_SUCCESS) {
    gf_file_info_free(tmp);
    gf_throw(rc);
  }
  if (gf_file_info_is_file(tmp)) {
    rc = file_info_set_hash(tmp);
    if (rc != GF_SUCCESS) {
      gf_file_info_free(tmp);
      gf_throw(rc);
    }
  } else {
    // TODO: memset must be wrapped by gf_memset
    memset(tmp->hash, 0, tmp->hash_size);
  }
  rc = file_info_prepare_children(tmp);
  if (rc != GF_SUCCESS) {
    gf_file_info_free(tmp);
    gf_throw(rc);
  }

  *info = tmp;
  
  return GF_SUCCESS;
}

static gf_status
file_info_scan(gf_file_info** info, const gf_path* path) {
  gf_status rc = 0;

  rc = gf_file_info_new(info, path);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  if (gf_file_info_is_directory(*info)) {
    DIR* dp = NULL;
    struct dirent* ep = NULL;

    dp = opendir(gf_path_get_string((*info)->full_path));
    if (!dp) {
      gf_raise(GF_E_API, "Couldn't open the directory.");
    }
    while ((ep = readdir(dp)) != NULL) {
      gf_file_info* child = NULL;
      gf_path* child_path = NULL;

      if (!strcmp(ep->d_name, ".") || !strcmp(ep->d_name, "..")) {
        continue;
      }
      rc = gf_path_append_string(&child_path, path, ep->d_name);
      if (rc != GF_SUCCESS) {
        (void)closedir(dp);
        gf_throw(rc);
      }
      rc = file_info_scan(&child, child_path);
      gf_path_free(child_path);
      if (rc != GF_SUCCESS) {
        (void)closedir(dp);
        gf_throw(rc);
      }
      rc = gf_file_info_add_child(*info, child);
      if (rc != GF_SUCCESS) {
        (void)closedir(dp);
        gf_throw(rc);
      }
    }
    (void)closedir(dp);
  }
  
  return GF_SUCCESS;
}

gf_status
gf_file_info_scan(gf_file_info** info, const gf_path* path) {
  gf_status rc = 0;
  
  gf_validate(info);
  gf_validate(path);

  rc = file_info_scan(info, path);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }

  return GF_SUCCESS;
}

void
gf_file_info_free(gf_file_info* info) {
  if (info) {
    if (info->file_name) {
      gf_path_free(info->file_name);
    }
    if (info->full_path) {
      gf_path_free(info->full_path);
    }
    if (info->children) {
      gf_array_free(info->children);
    }
    (void)file_info_init(info);
    gf_free(info);
  }
}

void
gf_file_info_free_any(gf_any* any) {
  if (any) {
    gf_file_info_free((gf_file_info*)(any->ptr));
  }
}

gf_bool
gf_file_info_is_file(const gf_file_info* info) {
  return info && S_ISREG(info->mode) ? GF_TRUE : GF_FALSE;
}

gf_bool
gf_file_info_is_directory(const gf_file_info* info) {
  return info && S_ISDIR(info->mode) ? GF_TRUE : GF_FALSE;
}

gf_size_t
gf_file_info_count_children(const gf_file_info* info) {
  return info && info->children ? gf_array_size(info->children) : 0;
}

gf_status
gf_file_info_add_child(gf_file_info* info, gf_file_info* child) {
  gf_any any = { 0 };
  
  gf_validate(info);
  gf_validate(child);

  any.ptr = child;
  _(gf_array_add(info->children, any));
  
  return GF_SUCCESS;
}

gf_status
gf_file_info_get_child(
  const gf_file_info* info, gf_size_t index, gf_file_info** child) {
  gf_any any = { 0 };
  
  gf_validate(info);
  gf_validate(info->children);
  gf_validate(child);

  _(gf_array_get(info->children, index, &any));
  *child = any.ptr;

  return GF_SUCCESS;
}

gf_status
gf_file_info_get_file_name(
  const gf_file_info* info, const gf_char** file_name) {
  gf_validate(info);
  gf_validate(file_name);

  *file_name = gf_path_get_string(info->file_name);

  return GF_SUCCESS;
}

gf_status
gf_file_info_get_full_path(
  const gf_file_info* info, const gf_char** full_path) {
  gf_validate(info);
  gf_validate(full_path);

  *full_path = gf_path_get_string(info->full_path);

  return GF_SUCCESS;
}

gf_status
gf_file_info_get_hash(const gf_file_info* info, gf_size_t size, gf_8u* hash) {
  gf_validate(info);
  gf_validate(size >= info->hash_size);
  gf_validate(hash);

  _(gf_memcpy(hash, info->hash, info->hash_size));

  return GF_SUCCESS;
}

gf_status
gf_file_info_get_hash_string(
  const gf_file_info* info, gf_size_t size, gf_8u* str) {
  gf_validate(info);
  gf_validate(size >= ((gf_size_t)(info->hash_size) * 2 + 1));
  gf_validate(str);

  for (gf_size_t i = 0; i < ((gf_size_t)(info->hash_size)); i++) {
    snprintf((gf_char*)&(str[i * 2]), 3, "%02x", info->hash[i]);
  }

  return GF_SUCCESS;
}

gf_status
gf_file_info_get_user_data(const gf_file_info* info, gf_any* user_data) {
  gf_validate(info);
  gf_validate(user_data);

  *user_data = info->user_data;
  
  return GF_SUCCESS;
}

gf_status
gf_file_info_get_user_flag(const gf_file_info* info, gf_32u* user_flag) {
  gf_validate(info);
  gf_validate(user_flag);

  *user_flag = info->user_flag;
  
  return GF_SUCCESS;
}

gf_status
gf_file_info_get_hash_size(const gf_file_info* info, gf_16u* hash_size) {
  gf_validate(info);
  gf_validate(hash_size);

  *hash_size = info->hash_size;

  return GF_SUCCESS;
}

gf_status
gf_file_info_get_inode(const gf_file_info* info, gf_16u* inode) {
  gf_validate(info);
  gf_validate(inode);

  *inode = info->inode;

  return GF_SUCCESS;
}

gf_status
gf_file_info_get_mode(const gf_file_info* info, gf_16u* mode) {
  gf_validate(info);
  gf_validate(mode);

  *mode = info->mode;

  return GF_SUCCESS;
}

gf_status
gf_file_info_get_link_count(const gf_file_info* info, gf_16s* link_count) {
  gf_validate(info);
  gf_validate(link_count);

  *link_count = info->link_count;
  
  return GF_SUCCESS;
}

gf_status
gf_file_info_get_uid(const gf_file_info* info, gf_16s* uid) {
  gf_validate(info);
  gf_validate(uid);

  *uid = info->uid;
  
  return GF_SUCCESS;
}

gf_status
gf_file_info_get_gid(const gf_file_info* info, gf_16s* gid) {
  gf_validate(info);
  gf_validate(gid);

  *gid = info->gid;
  
  return GF_SUCCESS;
}

gf_status
gf_file_info_get_device(const gf_file_info* info, gf_32u* device) {
  gf_validate(info);
  gf_validate(device);

  *device = info->device;

  return GF_SUCCESS;
}

gf_status
gf_file_info_get_rdevice(const gf_file_info* info, gf_32u* rdevice) {
  gf_validate(info);
  gf_validate(rdevice);

  *rdevice = info->rdevice;

  return GF_SUCCESS;
}

gf_status
gf_file_info_get_file_size(const gf_file_info* info, gf_64u* file_size) {
  gf_validate(info);
  gf_validate(file_size);

  *file_size = info->file_size;
  
  return GF_SUCCESS;
}

gf_status
gf_file_info_get_access_time(const gf_file_info* info, gf_64u* access_time) {
  gf_validate(info);
  gf_validate(access_time);

  *access_time = info->access_time;
  
  return GF_SUCCESS;
}

gf_status
gf_file_info_get_modify_time(const gf_file_info* info, gf_64u* modify_time) {
  gf_validate(info);
  gf_validate(modify_time);

  *modify_time = info->modify_time;

  return GF_SUCCESS;
}

gf_status
gf_file_info_get_create_time(const gf_file_info* info, gf_64u* create_time) {
  gf_validate(info);
  gf_validate(create_time);

  *create_time = info->create_time;

  return GF_SUCCESS;
}
