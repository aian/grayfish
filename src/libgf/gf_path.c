/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_path.c
** @brief Path management.
*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <windows.h>
#include <shlwapi.h>

#include <libgf/gf_swap.h>
#include <libgf/gf_memory.h>
#include <libgf/gf_string.h>
#include <libgf/gf_path.h>
#include <libgf/gf_cmd_config.h>

#include "gf_local.h"

struct gf_path {
  char* buf;
  gf_size_t len;
};

gf_status
path_init(gf_path* path) {
  gf_validate(path);

  path->buf = NULL;
  path->len = 0;

  return GF_SUCCESS;
}

gf_status
gf_path_new(gf_path** path, const char* path_str) {
  gf_status rc = 0;
  gf_path* tmp = NULL;

  gf_validate(path);

  _(gf_malloc((gf_ptr *)&tmp, sizeof(*tmp)));

  rc = path_init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    return rc;
  }
  rc = gf_path_set_string(tmp, path_str);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    return rc;
  }

  *path = tmp;

  return GF_SUCCESS;
}

void
gf_path_free(gf_path* path) {
  if (path) {
    if (path->buf) {
      gf_free(path->buf);
    }
    path->len = 0;
    gf_free(path);
  }
}

gf_status
gf_path_set_string(gf_path* path, const char* path_str) {
  const char* str = NULL;
  static const char nullstr[] = "";

  str = path_str ? path_str : nullstr;
  
  _(gf_strassign(&path->buf, str));
  path->len = gf_strlen(path->buf);
  
  return GF_SUCCESS;
}

const char* 
gf_path_get_string(const gf_path* path) {
  return path ? path->buf : NULL;
}

gf_status
gf_path_swap(gf_path* lhs, gf_path* rhs) {
  gf_validate(lhs);
  gf_validate(rhs);

  gf_swap(lhs->buf, rhs->buf);
  gf_swap(lhs->len, rhs->len);

  return GF_SUCCESS;
}

gf_status
gf_path_copy(gf_path* dst, const gf_path* src) {
  gf_validate(dst);
  gf_validate(src);

  _(gf_strassign(&dst->buf, src->buf));
  dst->len = src->len;
  
  return GF_SUCCESS;
}

gf_status
gf_path_clone(gf_path** dst, const gf_path* src) {
  gf_path* tmp = NULL;
  
  gf_validate(dst);
  gf_validate(src);

  _(gf_path_new(&tmp, gf_path_get_string(src)));
  *dst = tmp;
  
  return GF_SUCCESS;
}

gf_status
gf_path_canonicalize(gf_path* path) {
  gf_status rc = 0;
  BOOL ret = 0;
  char tmp[MAX_PATH] = { 0 };
  char *str = NULL;
    
  gf_validate(path);

  if (gf_path_is_empty(path)) {
    return GF_SUCCESS;
  }

  ret = PathCanonicalize(tmp, path->buf);
  if (!ret) {
    gf_raise(GF_E_API, "Failed to canonicalize the path string '%s'", path->buf);
  }
  rc = gf_strdup(&str, tmp);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  if (path->buf) {
    gf_free(path->buf);
    path->buf = NULL;
  }
  path->buf = str;
  path->len = gf_strlen(str);
  
  return GF_SUCCESS;
}

gf_status
gf_path_absolute_path(gf_path* path) {
  gf_status rc = 0;
  DWORD size = 0;
  char* buf = NULL;

  size = GetFullPathName(path->buf, 0, NULL, NULL);
  if (size <= 0) {
    gf_raise(GF_E_API, "Failed to build an absolute path.");
  }
  _(gf_malloc((gf_ptr*)&buf, size));

  GetFullPathName(path->buf, size, buf, NULL);

  rc = gf_strassign(&path->buf, buf);
  if (rc != GF_SUCCESS) {
    gf_free(buf);
    return rc;
  }
  path->len = gf_strlen(path->buf);

  return GF_SUCCESS;
}

gf_bool
gf_path_is_empty(const gf_path* path) {
  return !path || gf_strnull(path->buf);
}

gf_bool
gf_path_file_exists(const gf_path* path) {
  return path && PathFileExists(path->buf) ? GF_TRUE : GF_FALSE;
}

gf_bool
gf_path_is_directory(const gf_path* path) {
  return path && PathIsDirectory(path->buf) ? GF_TRUE : GF_FALSE;
}

gf_bool
gf_path_has_separator(const gf_path* path) {
  gf_bool ret = GF_FALSE;
  char* last = NULL;

  if (path && path->buf && path->len > 0) {
    last = &path->buf[path->len - 1];
    switch (*last) {
    case '\\':
    case '/':
      ret = GF_TRUE;
      break;
    default:
      ret = GF_FALSE;
      break;
    }
  }
  
  return ret;
}

gf_status
gf_path_append(gf_path* path, const gf_path* src) {
  char* buf = NULL;
  char* ptr = NULL;
  gf_size_t len = 0;
  
  gf_validate(path);
  gf_validate(src);

  if (gf_path_is_empty(src)) {
    return GF_SUCCESS;
  }
  /* Remove the last separator of path */
  if (gf_path_has_separator(path)) {
    path->buf[path->len - 1] = '\0';
    path->len -= 1;
  }
  /* 
  ** <path length> + <seaparator> + <src length>
  */
  if (path->len > 0) {
    len = path->len + 1 + src->len;
  } else {
    len = src->len;
  }
  _(gf_malloc((gf_ptr *)&buf, len + 1));
  /*
  ** Build up the new path
  */
  ptr = buf;
  if (path->len > 0) {
    memcpy_s(ptr, path->len, path->buf, path->len);
    ptr += path->len;
    ptr[0] = GF_PATH_SEPARATOR_CHAR;
    ptr++;
  }
  memcpy_s(ptr, src->len, src->buf, src->len);

  buf[len] = '\0';
  // Assignment
  gf_free(path->buf);
  path->buf = buf;
  path->len = len;
  
  return GF_SUCCESS;
}

gf_status
gf_path_append_string(gf_path** dst, const gf_path* src, const char* str) {
  gf_status rc = 0;
  gf_path* tmp = NULL;
  gf_path* appendix = NULL;
  
  gf_validate(dst);
  gf_validate(src);
  gf_validate(str);

  rc = gf_path_new(&tmp, gf_path_get_string(src));
  if (rc != GF_SUCCESS) {
    return rc;
  }
  rc = gf_path_new(&appendix, str);
  if (rc != GF_SUCCESS) {
    gf_path_free(tmp);
    return rc;
  }
  rc = gf_path_append(tmp, appendix);
  gf_path_free(appendix);
  if (rc != GF_SUCCESS) {
    gf_path_free(tmp);
    return rc;
  }
  *dst = tmp;
  
  return GF_SUCCESS;
}

gf_status
gf_path_evacuate(const gf_path* path) {
  gf_status rc = 0;
  gf_path* new_path = NULL;
  gf_size_t len = 0;
  char* buf = NULL;

  static gf_size_t extra = 256;
  
  gf_validate(path);

  /* No need to evacuate */
  if (!gf_path_file_exists(path)) {
    return GF_SUCCESS;
  }
  /* Make the evacuating path string */
  _(gf_path_new(&new_path, gf_path_get_string(path)));
  len = path->len + extra;
  rc = gf_malloc((gf_ptr *)buf, len);
  if (rc != GF_SUCCESS) {
    gf_path_free(new_path);
    return rc;
  }
  for (gf_size_t i = 0; ; i++) {
    int ret = 0;
    ret = sprintf_s(buf, len, "%s.old%04d", path->buf, i);
    if (ret == 0) {
      gf_path_free(new_path);
      gf_raise(GF_E_API, "Failed to evacuate the existing directory.");
    }
  }
  
  return GF_SUCCESS;
}

gf_status
gf_path_change_directory(const gf_path* path) {
  gf_validate(!gf_path_is_empty(path));

  if (!SetCurrentDirectory(path->buf)) {
    gf_raise(GF_E_INTERNAL,
             "Failed to change the directory. (%s)", path->buf);
  }

  return GF_SUCCESS;
}

gf_status
gf_path_create_directory(const gf_path* path) {
  const char* str = NULL;

  gf_validate(path);

  str = gf_path_get_string(path);
  if (!CreateDirectory(str, NULL)) {
    gf_raise(GF_E_PATH, "Failed to create directory. (%s)", path->buf);
  }
  
  return GF_SUCCESS;
}

gf_status
gf_path_get_module_file_path(gf_path** path) {
  gf_status rc = 0;
  DWORD path_size = MAX_PATH / 2 + 1;
  DWORD ret_size = 0;
  gf_path* new_path = NULL;
  char* tmp = NULL;
  
  gf_validate(path);

  do {
    path_size *= 2;

    rc = gf_realloc((gf_ptr *)&tmp, path_size);
    if (rc != GF_SUCCESS) {
      gf_free(tmp);
      return rc;
    }
    ret_size = GetModuleFileNameA(NULL, tmp, path_size);
    if (ret_size == 0) {
      gf_free(tmp);
      gf_raise(GF_E_PATH, "Failed to get the module file path.");
    }
  } while (ret_size >= path_size);

  rc = gf_path_new(&new_path, tmp);
  gf_free(tmp);
  if (rc != GF_SUCCESS) {
    return rc;
  }

  *path = new_path;

  return GF_SUCCESS;
}

gf_status
gf_path_get_module_directory_path(gf_path** path) {
  gf_status rc = 0;
  gf_size_t len = 0;
  gf_path* module_path = NULL;
  const char* module_string = NULL;
  gf_path* dir_path = NULL;
  gf_uri* uri = NULL;

  _(gf_path_get_module_file_path(&module_path));
  module_string = gf_path_get_string(module_path);
  assert(!gf_strnull(module_string));

  len = strlen(module_string) + 1;

  rc = gf_uri_split(&uri, module_string, len);
  gf_path_free(module_path);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  rc = gf_uri_build_directory_path(&dir_path, uri);
  gf_uri_free(uri);
  if (rc != GF_SUCCESS) {
    return rc;
  }

  *path = dir_path;

  return GF_SUCCESS;
}

gf_status
gf_path_get_style_path(gf_path** path) {
  gf_status rc = 0;
  gf_path* module_path = NULL;
  char* str = NULL;

  str = gf_config_get_string("site.style-path");
  if (gf_strnull(str)) {
    gf_raise(GF_E_PARAM, "The style path is empty");
  }

  if (PathIsRelative(str)) {
    rc = gf_path_get_module_directory_path(&module_path);
    if (rc != GF_SUCCESS) {
      gf_throw(rc);
    }
    rc = gf_path_append_string(path, module_path, str);
    gf_path_free(module_path);
    if (rc != GF_SUCCESS) {
      gf_throw(rc);
    }
    rc = gf_path_canonicalize(*path);
    if (rc != GF_SUCCESS) {
      gf_throw(rc);
    }
  } else {
    rc = gf_path_new(path, str);
    if (rc != GF_SUCCESS) {
      gf_throw(rc);
    }
  }
  
  return GF_SUCCESS;
}

gf_status
gf_path_get_current_path(gf_path** path) {
  gf_status rc = 0;
  DWORD len = 0;
  char* buf = NULL;
  gf_path* tmp = NULL;

  gf_validate(path);

  /* Determine the required buffer size */
  len = GetCurrentDirectory(0, NULL);

  _(gf_malloc((gf_ptr*)&buf, len));

  GetCurrentDirectory(len, buf);
  rc = gf_path_new(&tmp, buf);
  gf_free(buf);
  if (rc != GF_SUCCESS) {
    return rc;
  }

  *path = tmp;
  
  return GF_SUCCESS;
}

/* -------------------------------------------------------------------------- */

struct gf_uri {
  gf_size_t size;
  char* scheme;
  char* user;
  char* drive;
  char* dir;
  char* fname;
  char* ext;
};

static gf_status
uri_init(gf_uri* uri) {
  gf_validate(uri);

  uri->size = 0;
  uri->scheme = NULL;
  uri->user = NULL;
  uri->drive = NULL;
  uri->dir  = NULL;
  uri->fname = NULL;
  uri->ext = NULL;

  return GF_SUCCESS;
}

gf_status
gf_uri_new(gf_uri** uri, gf_size_t len) {
  gf_status rc = 0;
  gf_uri* tmp = NULL;

  gf_validate(uri);
  gf_validate(len > 0);
  
  _(gf_malloc((gf_ptr *)&tmp, sizeof(*tmp)));

  rc = uri_init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    return rc;
  }

  rc = gf_malloc((gf_ptr *)&tmp->drive, len);
  if (rc != GF_SUCCESS) {
    gf_uri_free(tmp);
    return rc;
  }
  rc = gf_malloc((gf_ptr *)&tmp->dir, len);
  if (rc != GF_SUCCESS) {
    gf_uri_free(tmp);
    return rc;
  }
  rc = gf_malloc((gf_ptr *)&tmp->fname, len);
  if (rc != GF_SUCCESS) {
    gf_uri_free(tmp);
    return rc;
  }
  rc = gf_malloc((gf_ptr *)&tmp->ext, len);
  if (rc != GF_SUCCESS) {
    gf_uri_free(tmp);
    return rc;
  }
  tmp->size = len;
  
  *uri = tmp;

  return GF_SUCCESS;
}

void
gf_uri_free(gf_uri* uri) {
  if (uri) {
    if (uri->drive) {
      gf_free(uri->drive);
      uri->drive = NULL;
    }
    if (uri->dir) {
      gf_free(uri->dir);
      uri->dir = NULL;
    }
    if (uri->fname) {
      gf_free(uri->fname);
      uri->fname = NULL;
    }
    if (uri->ext) {
      gf_free(uri->ext);
      uri->ext = NULL;
    }
    uri->size = 0;
    
    gf_free(uri);
  }
}

gf_status
gf_uri_split(gf_uri** uri, const char* path, gf_size_t len) {
  errno_t ret = 0;
  gf_uri* tmp = NULL;

  gf_validate(!gf_strnull(path));
  gf_validate(len > 0);

  _(gf_uri_new(&tmp, len));

  ret = _splitpath_s(
    path, tmp->drive, len, tmp->dir, len, tmp->fname, len, tmp->ext, len);
  if (ret != 0) {
    gf_uri_free(tmp);
    gf_raise(GF_E_PATH, "Failed to split the path.");
  }

  *uri = tmp;

  return GF_SUCCESS;
}

gf_status
gf_uri_build_directory_path(gf_path** path, gf_uri* uri) {
  gf_status rc = 0;
  gf_size_t size = uri->size * 2;
  gf_path* tmp = NULL;
  char* str = NULL;
  errno_t ret = 0;
  
  gf_validate(path);
  gf_validate(uri);

  _(gf_malloc((gf_ptr *)&str, size));

  ret = _makepath_s(str, size, uri->drive, uri->dir, NULL, NULL);
  if (ret != 0) {
    gf_free(str);
    gf_raise(GF_E_PATH, "Failed to make directory path.");
  }
  rc = gf_path_new(&tmp, str);
  gf_free(str);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  *path = tmp;
  
  return GF_SUCCESS;
}

gf_bool
gf_uri_match_extension(const char* path, const char* ext) {
  gf_status rc = 0;
  gf_uri* uri = NULL;
  
  if (!path) {
    return GF_FALSE;
  }
  rc = gf_uri_split(&uri, path, gf_strlen(path));
  if (rc != GF_SUCCESS) {
    return GF_FALSE;
  }

  return strcmp(uri->ext, ext) ? GF_FALSE : GF_TRUE;
}
