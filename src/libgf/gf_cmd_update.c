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
#include <libgf/gf_path.h>
#include <libgf/gf_site.h>
#include <libgf/gf_cmd_base.h>
#include <libgf/gf_cmd_update.h>

#include "gf_local.h"

struct gf_cmd_update {
  gf_cmd_base base;
  gf_site*    site;
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

  GF_CMD_UPDATE_CAST(cmd)->site = NULL;

  return GF_SUCCESS;
}

static gf_status
prepare(gf_cmd_base* cmd) {
  gf_validate(cmd);

  _(gf_cmd_base_set_info(cmd, &info_));
  
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

    if (GF_CMD_UPDATE_CAST(cmd)->site) {
      gf_site_free(GF_CMD_UPDATE_CAST(cmd)->site);
      GF_CMD_UPDATE_CAST(cmd)->site = NULL;
    }
    gf_free(cmd);
  }
}

static gf_status
update_read_site_file(gf_cmd_update* cmd) {
  if (gf_path_file_exists(GF_CMD_BASE_CAST(cmd)->site_path)) {
    _(gf_site_read_file(&cmd->site, GF_CMD_BASE_CAST(cmd)->site_path));
  } else {
    _(gf_site_new(&cmd->site));
  }
  return GF_SUCCESS;
}

static gf_status
update_scan_directory(gf_cmd_update* cmd) {
  gf_validate(cmd);
  gf_validate(!gf_path_is_empty(GF_CMD_BASE_CAST(cmd)->src_path));

  _(gf_site_update(cmd->site, GF_CMD_BASE_CAST(cmd)->src_path));
  
  return GF_SUCCESS;
}

static gf_status
update_write_file(gf_cmd_update* cmd) {
  gf_validate(cmd);

  _(gf_site_write_file(cmd->site, GF_CMD_BASE_CAST(cmd)->site_path));
  
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
