/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_setup.c
** @brief Initialize the system environment (private command).
*/
#include <assert.h>

#include <libgf/gf_countof.h>
#include <libgf/gf_memory.h>
#include <libgf/gf_string.h>
#include <libgf/gf_path.h>
#include <libgf/gf_cmd_base.h>
#include <libgf/gf_system.h>
#include <libgf/gf_config.h>
#include <libgf/gf_cmd_setup.h>

#include "gf_local.h"

struct gf_cmd_setup {
  gf_cmd_base base;      ///< Base class object
  gf_path*   root_path;  ///< Project root path
  gf_path*   conf_path;  ///< Local configuration path
};

/*!
**
*/

static const gf_cmd_base_info info_ = {
  .base = {
    .name        = "setup",
    .description = "Setup the new project",
    .args        = NULL,
    .create      = gf_cmd_setup_new,
    .free        = gf_cmd_setup_free,
    .execute     = gf_cmd_setup_execute,
  },
  .options = {
    /* Terminate */
    GF_OPTION_NULL,
  },
};

/*!
**
*/

static gf_status
init(gf_cmd_base* cmd) {
  gf_validate(cmd);

  /* Initialize the base class */
  _(gf_cmd_base_init(cmd));
  /* Initialize the members of this class */
  GF_CMD_SETUP_CAST(cmd)->root_path = NULL;
  GF_CMD_SETUP_CAST(cmd)->conf_path = NULL;

  return GF_SUCCESS;
}

static gf_status
prepare(gf_cmd_base* cmd) {
  gf_validate(cmd);

  _(gf_cmd_base_set_info(cmd, &info_));

  return GF_SUCCESS;
}

gf_status
gf_cmd_setup_new(gf_cmd_base** cmd) {
  gf_status rc = 0;
  gf_cmd_base* tmp = NULL;
  
  gf_validate(cmd);

  _(gf_malloc((gf_ptr *)&tmp, sizeof(gf_cmd_setup)));

  rc = init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    return rc;
  }
  rc = prepare(tmp);
  if (rc != GF_SUCCESS) {
    gf_cmd_setup_free(tmp);
    return rc;
  }

  *cmd = tmp;

  return GF_SUCCESS;
}

void
gf_cmd_setup_free(gf_cmd_base* cmd) {
  if (cmd) {
    gf_cmd_base_clear(GF_CMD_BASE_CAST(cmd));
    gf_free(cmd);
  }
}

static gf_status
setup_set_paths(gf_cmd_setup* cmd) {
  gf_status rc = 0;
  char* project_name = NULL;
  int args_count = 0;
  gf_path* root_path = NULL;
  gf_path* conf_path = NULL;

  gf_validate(cmd);
  
  args_count = gf_args_remain(cmd->base.args);
  if (args_count <= 0) {
    gf_raise(GF_E_OPTION, "Project name is not specified.");
  }
  /* Get the project name and build the project path */
  /*
  ** FIXME: 
  ** For now, project path is built with the specified project name. If the
  ** project name has special characters as a path string, it may occer
  ** unexpected effects.
  */
  _(gf_args_consume(cmd->base.args, &project_name));
  rc = gf_path_new(&root_path, project_name);
  gf_free(project_name);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  _(gf_path_canonicalize(root_path));
  _(gf_path_absolute_path(root_path));

  if (!gf_path_is_empty(cmd->root_path)) {
    gf_path_free(cmd->root_path);
    cmd->root_path = NULL;
  }
  cmd->root_path = root_path;

  /* Get the config path */
  _(gf_path_append_string(&conf_path, cmd->root_path, GF_CONFIG_DIRECTORY));
  if (!gf_path_is_empty(cmd->conf_path)) {
    gf_path_free(cmd->conf_path);
    cmd->conf_path = NULL;
  }
  cmd->conf_path = conf_path;
  
  return GF_SUCCESS;
}

static gf_status
setup_create_config_file(gf_cmd_setup* cmd) {
  gf_status rc = 0;
  gf_path* path = NULL;
  
  gf_validate(cmd);

  _(gf_path_append_string(&path, cmd->conf_path, GF_CONFIG_FILE_NAME));
  rc = gf_config_write_file(path);
  if (rc != GF_SUCCESS) {
    gf_error("Failed to create Config file '%s'.", gf_path_get_string(path));
    gf_path_free(path);
    return rc;
  }
  gf_info("Config file '%s' created.", gf_path_get_string(path));
  gf_path_free(path);
  
  return GF_SUCCESS;
}

static gf_status
setup_create_project_directory(gf_cmd_setup* cmd) {
  const char* path = NULL;
  
  gf_validate(cmd);
  gf_validate(!gf_path_is_empty(cmd->root_path));
  gf_validate(!gf_path_is_empty(cmd->conf_path));
  
  /* Create a project directory */
  path = gf_path_get_string(cmd->root_path);
  if (!gf_path_file_exists(cmd->root_path)) {
    _(gf_path_create_directory(cmd->root_path));
    gf_info("Directory '%s' created.", path);
  } else {
    gf_warn("Directory '%s' already exists.", path);
  }
  /* Create the config directory */
  path = gf_path_get_string(cmd->conf_path);
  if (!gf_path_file_exists(cmd->conf_path)) {
    _(gf_path_create_directory(cmd->conf_path));
    gf_info("Directory '%s' created.", path);
  } else {
    gf_warn("Directory '%s' already exists.", path);
  }
  
  return GF_SUCCESS;
}

static gf_status
setup_create_document_directories(gf_cmd_setup* cmd) {
  gf_status rc = 0;
  char* str = NULL;
  gf_path* pub_path = NULL;
  gf_path* src_path = NULL;
  
  gf_validate(cmd);
  gf_validate(!gf_path_is_empty(cmd->root_path));

  /* Publish path */
  str = gf_config_get_string("site.pub-path");
  if (!gf_strnull(str)) {
    rc = gf_path_append_string(&pub_path, cmd->root_path, str);
    gf_free(str);
    if (rc != GF_SUCCESS) {
      return rc;
    }
    if (!gf_path_file_exists(pub_path)) {
      rc = gf_path_create_directory(pub_path);
      if (rc != GF_SUCCESS) {
        gf_path_free(pub_path);
        return rc;
      }
      gf_info("Directory '%s' created.", gf_path_get_string(pub_path));
    } else {
      gf_warn("Directory '%s' already exists.", gf_path_get_string(pub_path));
    }
    gf_path_free(pub_path);
  } else {
    gf_warn("The publish path is not defined. Skipped.");
  }

  /* Source path */
  str = gf_config_get_string("site.src-path");
  if (!gf_strnull(str)) {
    rc = gf_path_append_string(&src_path, cmd->root_path, str);
    gf_free(str);
    if (rc != GF_SUCCESS) {
      return rc;
    }
    if (!gf_path_file_exists(src_path)) {
      rc = gf_path_create_directory(src_path);
      if (rc != GF_SUCCESS) {
        gf_path_free(src_path);
        return rc;
      }
      gf_info("Directory '%s' created.", gf_path_get_string(src_path));
    } else {
      gf_warn("Directory '%s' already exists.", gf_path_get_string(src_path));
    }
    gf_path_free(src_path);
  } else {
    gf_warn("The source path is not defined. Skipped.");
  }

  return GF_SUCCESS;
}

static gf_status
setup_process(gf_cmd_setup* cmd) {
  gf_validate(cmd);

  /* Set paths */
  _(setup_set_paths(cmd));

  /* Create a project_directory */
  _(setup_create_project_directory(cmd));
  /* Create a config files */
  _(setup_create_config_file(cmd));
  /* Create document paths */
  _(setup_create_document_directories(cmd));
  
  return GF_SUCCESS;
}

/*!
** This function tries to create a new directory. If the directory path, which
** is specified by the project name, already exist, it renames the existing one
** into the other name.
*/

gf_status
gf_cmd_setup_execute(gf_cmd_base* cmd) {
  gf_validate(cmd);

  _(gf_args_parse(cmd->args));
  _(setup_process(GF_CMD_SETUP_CAST(cmd)));

  return GF_SUCCESS;
}
