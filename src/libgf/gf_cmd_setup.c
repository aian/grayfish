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

/*!
** @brief The sub-command handle for `setup' command
**
** Typically, the path gf_cmd_setup::base.root_path is expected to be a project
** path, however, it should not be in the project path in this command. The
** project path, wihch we are intended to create is represented with the member
** variable gf_cmd_setup::proj_path.
**
** The path gf_cmd_base::root_path is used as a current working path and
** gf_cmd_base::conf_path are not used in this command because these paths are
** automatically generated assuming the path gf_cmd_base::root_path is a valid
** project path.
*/

struct gf_cmd_setup {
  gf_cmd_base base;      ///< Base class object
  gf_path*    proj_path; ///< Project path to be created
  gf_path*    conf_path; ///< Config path to be crated
  gf_path*    conf_file; ///< Config file path to be crated
  gf_bool     help;
};

/*!
** @brief
*/

enum {
  OPT_SETUP_HELP,
};

/*!
** @brief META information for the `setup' sub-command
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
    {
      .key         = OPT_SETUP_HELP,
      .opt_short   = 'h',
      .opt_long    = "help",
      .opt_count   = 0,
      .usage       = "-h, --help",
      .description = "Show help.",
    },
    /* Terminate */
    GF_OPTION_NULL,
  },
};

/*!
** @brief Initialize the command state
*/

static gf_status
init(gf_cmd_base* cmd) {
  gf_validate(cmd);

  /* Initialize the base class */
  _(gf_cmd_base_init(cmd));
  /* Initialize this class */
  GF_CMD_SETUP_CAST(cmd)->proj_path = NULL;
  GF_CMD_SETUP_CAST(cmd)->conf_path = NULL;
  GF_CMD_SETUP_CAST(cmd)->conf_file = NULL;
  GF_CMD_SETUP_CAST(cmd)->help = GF_FALSE;

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

/* -------------------------------------------------------------------------- */

static void
setup_show_help(const gf_cmd_setup* cmd) {
  gf_msg("usage: gf [options] setup [--help] [[options] <site name>]");
  gf_msg("");
  assert(cmd);
  if (cmd && GF_CMD_BASE_CAST(cmd)->args) {
    gf_msg("Options:");
    gf_msg("");
    gf_args_print_help(GF_CMD_BASE_CAST(cmd)->args);
  }
  gf_msg("");
}

static gf_status
setup_check_args(const gf_cmd_setup* cmd) {
  int remain = 0;
  
  gf_validate(cmd);

  remain = gf_args_remain(GF_CMD_BASE_CAST(cmd)->args);
  if (remain != 1) {
    setup_show_help(cmd);
    gf_raise(GF_E_OPTION, "Invalid command.");
  }
  
  return GF_SUCCESS;
}

/*
** @brief Check if the root directory is a valid path to setup
**
** The `setup' sub-command expects the root directory not to be in the other
** project directory and so on.
**
** @param [in] cmd The handle of the sub-command `setup'.
** @return GF_SUCCESS if the path is valid to setup, otherwise GF_E_*
*/

static gf_status
setup_check_root_path(gf_cmd_setup* cmd) {
  gf_bool ret = GF_FALSE;
  const char* s = NULL;

  gf_validate(cmd);

  /* Check if the path is empty */
  ret = gf_path_is_empty(GF_CMD_BASE_CAST(cmd)->root_path);
  if (ret) {
    gf_raise(GF_E_INTERNAL, "The current path is empty.");
  }
  /* Check if the path is not the other project directory */
  s = gf_path_get_string(GF_CMD_BASE_CAST(cmd)->root_path);
  ret = gf_cmd_base_is_root_project_directory(GF_CMD_BASE_CAST(cmd));
  if (ret) {
    gf_raise(GF_E_COMMAND, "This path is other the project directory. (%s)", s);
  }

  return GF_SUCCESS;
}

/*!
** @brief Make up the project paths
**
** @note This function consumes one argument as a project name.
**
** @param [in] cmd The `setup' command handle
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

static gf_status
setup_make_paths(gf_cmd_setup* cmd) {
  gf_status rc = 0;
  gf_path* root = NULL;
  gf_path* proj = NULL;
  gf_path* conf = NULL;
  gf_path* file = NULL;
  char* name = NULL;

  gf_validate(cmd);

  /* Project path */
  _(gf_args_consume(GF_CMD_BASE_CAST(cmd)->args, &name));
  // TODO: check if the name is valid for project path
  root = GF_CMD_BASE_CAST(cmd)->root_path;
  assert(!gf_path_is_empty(root));
  _(gf_path_append_string(&proj, root, name));

  /* Config path */
  rc = gf_path_append_string(&conf, proj, GF_CONFIG_DIRECTORY);
  if (rc != GF_SUCCESS) {
    gf_path_free(proj);
    gf_throw(rc);
  }

  /* Config file path */
  rc = gf_path_append_string(&file, conf, GF_CONFIG_FILE_NAME);
  if (rc != GF_SUCCESS) {
    gf_path_free(proj);
    gf_path_free(conf);
    gf_throw(rc);
  }
  
  if (cmd->proj_path) {
    gf_path_free(cmd->proj_path);
  }
  cmd->proj_path = proj;

  if (cmd->conf_path) {
    gf_path_free(cmd->conf_path);
  }
  cmd->conf_path = conf;

  if (cmd->conf_file) {
    gf_path_free(cmd->conf_file);
  }
  cmd->conf_file = file;
  
  return GF_SUCCESS;
}

static gf_status
setup_create_config_file(gf_cmd_setup* cmd) {
  gf_status rc = 0;
  const char* path = NULL;
  
  gf_validate(cmd);
  gf_validate(!gf_path_is_empty(cmd->conf_file));

  path = gf_path_get_string(cmd->conf_file);
  
  rc = gf_config_write_file(cmd->conf_file);
  if (rc != GF_SUCCESS) {
    gf_error("Failed to create Config file `%s'.", path);
    return rc;
  }
  gf_info("Config file '%s' has been created.", path);

  return GF_SUCCESS;
}

static gf_status
setup_create_project_directory(gf_cmd_setup* cmd) {
  const char* path = NULL;
  
  gf_validate(cmd);
  
  /* Create a project directory */
  path = gf_path_get_string(cmd->proj_path);
  if (!gf_path_file_exists(cmd->proj_path)) {
    _(gf_path_create_directory(cmd->proj_path));
    gf_info("Directory '%s' has been created.", path);
  } else {
    gf_warn("Directory '%s' already exists. (not created)", path);
  }
  /* Create the config directory */
  path = gf_path_get_string(cmd->conf_path);
  if (!gf_path_file_exists(cmd->conf_path)) {
    _(gf_path_create_directory(cmd->conf_path));
    gf_info("Directory '%s' has been created.", path);
  } else {
    gf_warn("Directory '%s' already exists. (not created)", path);
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
  gf_validate(!gf_path_is_empty(cmd->proj_path));

  /* Publish path */
  str = gf_config_get_string("site.pub-path");
  if (!gf_strnull(str)) {
    rc = gf_path_append_string(&pub_path, cmd->proj_path, str);
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
      gf_info("Directory '%s' has been created.",
              gf_path_get_string(pub_path));
    } else {
      gf_warn("Directory '%s' already exists. (not created)",
              gf_path_get_string(pub_path));
    }
    gf_path_free(pub_path);
  } else {
    gf_warn("The publish path is not defined. Skipped.");
  }

  /* Source path */
  str = gf_config_get_string("site.src-path");
  if (!gf_strnull(str)) {
    rc = gf_path_append_string(&src_path, cmd->proj_path, str);
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
      gf_info("Directory '%s' has been created.",
              gf_path_get_string(src_path));
    } else {
      gf_warn("Directory '%s' already exists. (not created)",
              gf_path_get_string(src_path));
    }
    gf_path_free(src_path);
  } else {
    gf_warn("The source path is not defined. Skipped.");
  }

  return GF_SUCCESS;
}

static gf_status
setup_process(gf_cmd_setup* cmd) {
  gf_status rc = 0;
  
  gf_validate(cmd);

  /* Check if the current directory is not in the project path */
  rc = setup_check_root_path(cmd);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  /* Make a project path */
  rc = setup_make_paths(cmd);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  /* Create a project_directory */
  rc = setup_create_project_directory(cmd);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  /* Create a config files */
  rc = setup_create_config_file(cmd);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  /* Create document paths */
  rc = setup_create_document_directories(cmd);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  
  return GF_SUCCESS;
}

static gf_status
setup_set_options(gf_cmd_setup* cmd) {
  /* Parse command line arguments */
  _(gf_args_parse(cmd->base.args));
  cmd->help = gf_args_is_specified(cmd->base.args, OPT_SETUP_HELP);
  return GF_SUCCESS;
}

/*!
** This function tries to create a new directory. If the directory path, which
** is specified by the project name, already exist, we do nothing on this path.
*/

gf_status
gf_cmd_setup_execute(gf_cmd_base* cmd) {
  gf_status rc = 0;
  
  gf_validate(cmd);

  rc = setup_set_options(GF_CMD_SETUP_CAST(cmd));
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  rc = setup_check_args(GF_CMD_SETUP_CAST(cmd));
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }

  gf_msg("Setting up your site ...");

  rc = setup_process(GF_CMD_SETUP_CAST(cmd));
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }

  gf_msg("Done.");

  return GF_SUCCESS;
}
