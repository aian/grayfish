/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_system.h
** @brief System management.
*/
#include <assert.h>
#include <string.h>

#include <libgf/gf_string.h>
#include <libgf/gf_system.h>

#include "gf_local.h"

gf_bool
gf_system_is_project_path(const gf_path* path) {
  gf_bool ret = GF_FALSE;
  gf_path* full_path = NULL;

  _(gf_path_append_string(&full_path, path, GF_PATH_SYSTEM_DIR));
  ret = gf_path_file_exists(full_path);
  gf_path_free(full_path);
  
  return ret;
}

gf_status
gf_system_make_project(const gf_path* base, const char* name) {
  gf_status rc = 0;
  gf_path* path = NULL;
  
  gf_validate(base);
  gf_validate(!gf_strnull(name));

  if (!gf_path_is_directory(base)) {
    gf_raise(GF_E_PATH, "The path to create the project is not a directory.");
  }
  /* Project path */
  _(gf_path_append_string(&path, base, name));
  /* Evacuate the existing file */
  rc = gf_path_evacuate(path);
  if (rc != GF_SUCCESS) {
    gf_path_free(path);
    return rc;
  }
  /* Create project directory */
  rc = gf_path_create_directory(path);
  gf_path_free(path);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  
  return GF_SUCCESS;
}

gf_status
gf_system_get_system_config_path(gf_path** path) {
  gf_status rc = 0;
  gf_path* module_path = NULL;
  
  gf_validate(path);

  _(gf_path_get_module_directory_path(&module_path));

  rc = gf_path_append_string(path, module_path, ".." GF_PATH_SEPARATOR "etc");
  gf_path_free(module_path);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  rc = gf_path_canonicalize(*path);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  
  return GF_SUCCESS;
}

gf_status
gf_system_get_system_config_file_path(gf_path** path) {
  gf_status rc = 0;
  gf_path* tmp = NULL;
  
  _(gf_system_get_system_config_path(&tmp));
  rc = gf_path_append_string(path, tmp, GF_CONFIG_FILE_NAME);
  if (rc != GF_SUCCESS) {
    gf_path_free(tmp);
    return rc;
  }
  
  return GF_SUCCESS;
}
