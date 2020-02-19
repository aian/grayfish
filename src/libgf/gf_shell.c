/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_shell.h
** @brief Shell operations.
*/

#include <assert.h>

#include <windows.h>

#include <libgf/gf_string.h>
#include <libgf/gf_shell.h>
#include <libgf/gf_local.h>

/* -------------------------------------------------------------------------- */

/*
** The common interface for file attributes
*/

static DWORD
shell_get_file_attributes(const gf_path* path) {
  const char* s = NULL;
  DWORD ret = 0;
  
  if (gf_path_is_empty(path)) {
    return INVALID_FILE_ATTRIBUTES;
  }
  s = gf_path_get_string(path);
  assert(!gf_strnull(s));
  ret = GetFileAttributes(s);

  return ret;
}

static gf_bool
shell_has_specified_attributes(DWORD attr, DWORD flags) {
  return ((attr != INVALID_FILE_ATTRIBUTES) && (attr & flags))
    ? GF_TRUE : GF_FALSE;
}

static gf_bool
shell_has_attributes(const gf_path* path, DWORD flags) {
  DWORD attr = 0;

  if (gf_path_is_empty(path)) {
    return GF_FALSE;
  }
  attr = shell_get_file_attributes(path);

  return shell_has_specified_attributes(attr, flags);
}

/* -------------------------------------------------------------------------- */

gf_bool
gf_shell_is_file_exists(const gf_path* path) {
  DWORD attr = 0;
  
  if (gf_path_is_empty(path)) {
    return GF_FALSE;
  }
  attr = shell_get_file_attributes(path);
  
  return attr != INVALID_FILE_ATTRIBUTES ? GF_TRUE : GF_FALSE;
}

struct gf_file_info {
  gf_path* path;
};

gf_status
gf_shell_get_file_info(gf_file_info** info, const gf_path* path) {
  gf_validate(info);
  gf_validate(!gf_path_is_empty(path));
  return GF_SUCCESS;
}

gf_bool
gf_shell_is_directory(const gf_path* path) {
  return shell_has_attributes(path, FILE_ATTRIBUTE_DIRECTORY);
}

gf_bool
gf_shell_is_normal_file(const gf_path* path) {
  return shell_has_attributes(path, FILE_ATTRIBUTE_NORMAL);
}

gf_status
gf_shell_copy_file(const gf_path* dst, const gf_path* src) {
  BOOL ret = FALSE;
  const char* s = NULL;
  const char* d = NULL;

  static const DWORD flags = 0;
  
  gf_validate(!gf_path_is_empty(dst));
  gf_validate(!gf_path_is_empty(src));

  s = gf_path_get_string(src);
  d = gf_path_get_string(dst);

  ret = CopyFileEx(s, d, NULL, NULL, FALSE, flags);
  if (!ret) {
    gf_raise(GF_E_SHELL, "Failed to copy file (src:%s)(dst:%s)", s, d);
  }
  
  return GF_SUCCESS;
}

gf_status
gf_shell_make_directory(const gf_path* path) {
  BOOL ret = FALSE;
  const char* s = NULL;

  gf_validate(!gf_path_is_empty(path));

  s = gf_path_get_string(path);
  ret = CreateDirectory(s, NULL);
  if (!ret) {
    gf_raise(GF_E_SHELL, "Failed to create directory. (%s)", s);
  }

  return GF_SUCCESS;
}

gf_status
gf_shell_touch(const gf_path* path) {
  gf_validate(!gf_path_is_empty(path));
  return GF_SUCCESS;
}

gf_status
gf_shell_remove_file(const gf_path* path) {
  BOOL ret = FALSE;
  const char* s = NULL;
  
  gf_validate(!gf_path_is_empty(path));

  s = gf_path_get_string(path);
  ret = DeleteFile(s);
  if (!ret) {
    gf_raise(GF_E_SHELL, "Faild to remove file. (%s)", s);
  }
  
  return GF_SUCCESS;
}

gf_status
gf_shell_remove_direcroty(const gf_path* path) {
  BOOL ret = FALSE;
  const char* s = NULL;
  
  gf_validate(!gf_path_is_empty(path));

  s = gf_path_get_string(path);
  ret = RemoveDirectory(s);
  if (!ret) {
    gf_raise(GF_E_SHELL, "Failed to remove direcotry. (%s)", s);
  }
  
  return GF_SUCCESS;
}

gf_status
gf_shell_rename(const gf_path* dst, const gf_path* src) {
  BOOL ret = FALSE;
  const char* s = NULL;
  const char* d = NULL;
  static const DWORD flags = 0;
  
  gf_validate(!gf_path_is_empty(dst));
  gf_validate(!gf_path_is_empty(src));

  s = gf_path_get_string(src);
  d = gf_path_get_string(dst);
  ret = MoveFileEx(s, d, flags);
  if (!ret) {
    gf_raise(GF_E_SHELL, "Failed to move file. (src:%s)(dst:%s)", s, d);
  }
  
  return GF_SUCCESS;
}

gf_status
gf_shell_move(const gf_path* dst, const gf_path* src) {
  return gf_shell_rename(dst, src);
}

gf_status
gf_shell_traverse_tree(
  const gf_path* path, gf_shell_traverse_order order, gf_shell_fn fn, gf_ptr data) {

  HANDLE finder = INVALID_HANDLE_VALUE;
  WIN32_FIND_DATA find_data = { 0 };
  
  gf_path* find_path = NULL;

  static const char WILDCARD[]    = "*.*";
  static const char CURRENT_DIR[] = ".";
  static const char PARENT_DIR[]  = "..";
  
  gf_validate(!gf_path_is_empty(path));

  _(gf_path_append_string(&find_path, path, WILDCARD));
  assert(!gf_path_is_empty(find_path));

  /* Search files */
  finder = FindFirstFile(gf_path_get_string(find_path), &find_data);
  if (finder == INVALID_HANDLE_VALUE) {
    gf_path_free(find_path);
    gf_raise(GF_E_SHELL, "Failed to find file. (%s)", gf_path_get_string(path));
  }
  do {
    gf_status rc = 0;
    const char* name = find_data.cFileName;        // alias
    const DWORD attr = find_data.dwFileAttributes; // alias
    
    if (!strcmp(name, CURRENT_DIR) || !strcmp(name, PARENT_DIR)) {
      continue;
    }
    /* Preorder processing */
    if (order == GF_SHELL_TRAVERSE_PREORDER) {
      if (fn) {
        rc = fn(path, &find_data, data);
        if (rc != GF_SUCCESS) {
          gf_path_free(find_path);
          FindClose(finder);
          gf_throw(rc);
        }
      }
    }
    /* Dig into the sub-directory */
    if (shell_has_specified_attributes(attr, FILE_ATTRIBUTE_DIRECTORY)) {
      gf_path* subdir = NULL;

      rc = gf_path_append_string(&subdir, path, name);
      if (rc != GF_SUCCESS) {
        gf_path_free(find_path);
        FindClose(finder);
        gf_throw(rc);
      }
      rc = gf_shell_traverse_tree(subdir, order, fn, data);
      if (rc != GF_SUCCESS) {
        gf_path_free(find_path);
        FindClose(finder);
        gf_throw(rc);
      }
    }
    /* Postorder processing */
    if (order == GF_SHELL_TRAVERSE_POSTORDER) {
      if (fn) {
        rc = fn(path, &find_data, data);
        if (rc != GF_SUCCESS) {
          gf_path_free(find_path);
          FindClose(finder);
          gf_throw(rc);
        }
      }
    }
  } while (FindNextFile(finder, &find_data));

  gf_path_free(find_path);
  FindClose(finder);

  return GF_SUCCESS;
}

gf_status
gf_shell_copy_tree(const gf_path* dst, const gf_path* src) {
  gf_validate(!gf_path_is_empty(dst));
  gf_validate(!gf_path_is_empty(src));

  return GF_SUCCESS;
}

gf_status
gf_shell_remove_tree(const gf_path* path) {
  gf_validate(!gf_path_is_empty(path));

  return GF_SUCCESS;
}

gf_status
gf_shell_change_directory(const gf_path* path) {
  BOOL ret = FALSE;
  const char* s = NULL;
  
  gf_validate(!gf_path_is_empty(path));
  s = gf_path_get_string(path);
  ret = SetCurrentDirectory(s);
  if (!ret) {
    gf_raise(GF_E_SHELL, "Failed to change directory. (%s)", s);
  }

  return GF_SUCCESS;
}
