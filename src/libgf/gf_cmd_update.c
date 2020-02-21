/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_update.c
** @brief Initialize the system environment (private command).
*/
#include <assert.h>

#include <libgf/gf_countof.h>
#include <libgf/gf_memory.h>
#include <libgf/gf_string.h>
#include <libgf/gf_path.h>
#include <libgf/gf_cmd_config.h>
#include <libgf/gf_site.h>
#include <libgf/gf_system.h>
#include <libgf/gf_cmd_base.h>
#include <libgf/gf_cmd_update.h>

#include "gf_local.h"

struct gf_cmd_update {
  gf_cmd_base base;
  gf_path*   root_path;
  gf_path*   conf_path;
  gf_path*   site_path;
  gf_path*   src_path;
  gf_site*   site;
};

enum {
  OPT_BUILD_OPTIONS,
};

static const gf_cmd_base_info info_ = {
  .base = {
    .name        = "update",
    .description = "Update the status of the documents",
    .args        = NULL,
    .create      = gf_cmd_update_new,
    .free        = gf_cmd_update_free,
    .execute     = gf_cmd_update_execute,
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

  _(gf_cmd_base_init(cmd));

  GF_CMD_UPDATE_CAST(cmd)->root_path = NULL;
  GF_CMD_UPDATE_CAST(cmd)->conf_path = NULL;
  GF_CMD_UPDATE_CAST(cmd)->site_path = NULL;
  GF_CMD_UPDATE_CAST(cmd)->src_path = NULL;
  GF_CMD_UPDATE_CAST(cmd)->site = NULL;

  return GF_SUCCESS;
}

/*
** NOTE: This function is almost the same as gf_cmd_update.c:update_make_paths(),
** We should refactor to call this as a common function.
*/

static gf_status
update_make_paths(gf_cmd_update* cmd) {
  gf_status rc = 0;
  gf_path* path = NULL;
  char* str = NULL;
  
  gf_validate(cmd);

  /* Check if the path here is the project directory */
  _(gf_path_get_current_path(&path));
  if (!gf_system_is_project_path(path)) {
    gf_path_free(path);
    gf_raise(GF_E_STATE, "The current path is not a project directory.");
  }
  cmd->root_path = path;
  /* Config path */
  _(gf_path_append_string(&cmd->conf_path, cmd->root_path, GF_CONFIG_DIRECTORY));
  assert(gf_path_file_exists(cmd->conf_path));
  /* Source path */
  str = gf_config_get_string("site.src-path");
  if (gf_strnull(str)) {
    gf_raise(GF_E_PARAM, "The source path is not specified on config file");
  }
  rc = gf_path_append_string(&cmd->src_path, cmd->root_path, str);
  gf_free(str);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  /* Site file path */
  /* NOTE: We don't test whether the site file path exists. */
  _(gf_path_append_string(&cmd->site_path, cmd->conf_path, GF_SITE_FILE_NAME));

  return GF_SUCCESS;
}

static gf_status
prepare(gf_cmd_base* cmd) {
  gf_validate(cmd);

  _(gf_cmd_base_set_info(cmd, &info_));
  _(update_make_paths(GF_CMD_UPDATE_CAST(cmd)));
  
  return GF_SUCCESS;
}

gf_status
gf_cmd_update_new(gf_cmd_base** cmd) {
  gf_status rc = 0;
  gf_cmd_base* tmp = NULL;
  
  gf_validate(cmd);

  _(gf_malloc((gf_ptr *)&tmp, sizeof(gf_cmd_update)));

  rc = init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    return rc;
  }
  rc = prepare(tmp);
  if (rc != GF_SUCCESS) {
    gf_cmd_update_free(tmp);
    return rc;
  }

  *cmd = tmp;

  return GF_SUCCESS;
}

void
gf_cmd_update_free(gf_cmd_base* cmd) {
  if (cmd) {
    gf_cmd_base_clear(GF_CMD_BASE_CAST(cmd));

    if (GF_CMD_UPDATE_CAST(cmd)->root_path) {
      gf_path_free(GF_CMD_UPDATE_CAST(cmd)->root_path);
      GF_CMD_UPDATE_CAST(cmd)->root_path = NULL;
    }
    if (GF_CMD_UPDATE_CAST(cmd)->conf_path) {
      gf_path_free(GF_CMD_UPDATE_CAST(cmd)->conf_path);
      GF_CMD_UPDATE_CAST(cmd)->conf_path = NULL;
    }
    if (GF_CMD_UPDATE_CAST(cmd)->site_path) {
      gf_path_free(GF_CMD_UPDATE_CAST(cmd)->site_path);
      GF_CMD_UPDATE_CAST(cmd)->site_path = NULL;
    }
    if (GF_CMD_UPDATE_CAST(cmd)->src_path) {
      gf_path_free(GF_CMD_UPDATE_CAST(cmd)->src_path);
      GF_CMD_UPDATE_CAST(cmd)->src_path = NULL;
    }

    if (GF_CMD_UPDATE_CAST(cmd)->site) {
      gf_site_free(GF_CMD_UPDATE_CAST(cmd)->site);
      GF_CMD_UPDATE_CAST(cmd)->site = NULL;
    }
    gf_free(cmd);
  }
}

static gf_status
update_read_site_file(gf_cmd_update* cmd) {
  if (gf_path_file_exists(cmd->site_path)) {
    _(gf_site_read_file(&cmd->site, cmd->site_path));
  } else {
    _(gf_site_new(&cmd->site));
  }
  return GF_SUCCESS;
}

static gf_status
update_scan_directory(gf_cmd_update* cmd) {
  gf_validate(cmd);
  gf_validate(!gf_path_is_empty(cmd->src_path));

  _(gf_site_update(cmd->site, cmd->src_path));
  
  return GF_SUCCESS;
}

static gf_status
update_write_file(gf_cmd_update* cmd) {
  gf_validate(cmd);

  _(gf_site_write_file(cmd->site, cmd->site_path));
  
  return GF_SUCCESS;
}

static gf_status
update_process(gf_cmd_update* cmd) {
  gf_validate(cmd);

  /* Read the existing site file */
  _(update_read_site_file(cmd));
  /* Scan directory */
  _(update_scan_directory(cmd));
  /* Write site file */
  _(update_write_file(cmd));
  
  return GF_SUCCESS;
}

gf_status
gf_cmd_update_execute(gf_cmd_base* cmd) {
  gf_validate(cmd);

  gf_msg("Update the project directory ...");

  _(update_process(GF_CMD_UPDATE_CAST(cmd)));

  gf_msg("Done.");

  return GF_SUCCESS;
}
