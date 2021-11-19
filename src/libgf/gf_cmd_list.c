/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_list.c
** @brief Module list.
*/

#include <libgf/gf_countof.h>
#include <libgf/gf_memory.h>
#include <libgf/gf_string.h>
#include <libgf/gf_path.h>
#include <libgf/gf_cmd_config.h>
#include <libgf/gf_system.h>
#include <libgf/gf_site.h>
#include <libgf/gf_cmd_list.h>

#include "gf_local.h"

struct gf_cmd_list {
  gf_cmd_base base;
  gf_site*    site;
};


/*!
** @brief
**
*/

static const gf_cmd_base_info info_ = {
  .base = {
    .name        = "list",
    .description = "List document status",
    .args        = NULL,
    .create      = gf_cmd_list_new,
    .free        = gf_cmd_list_free,
    .execute     = gf_cmd_list_execute,
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

  GF_CMD_LIST_CAST(cmd)->site = NULL;

  return GF_SUCCESS;
}

static gf_status
prepare(gf_cmd_base* cmd) {
  gf_validate(cmd);

  _(gf_cmd_base_set_info(cmd, &info_));

  return GF_SUCCESS;
}

gf_status
gf_cmd_list_new(gf_cmd_base** cmd) {
  gf_status rc = 0;
  gf_cmd_base* tmp = NULL;

  _(gf_malloc((gf_ptr*)&tmp, sizeof(gf_cmd_list)));

  rc = init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    return rc;
  }
  rc = prepare(tmp);
  if (rc != GF_SUCCESS) {
    gf_cmd_list_free(tmp);
    return rc;
  }

  *cmd = tmp;
  
  return GF_SUCCESS;
}

void
gf_cmd_list_free(gf_cmd_base* cmd) {
  if (cmd) {
    gf_cmd_base_clear(GF_CMD_BASE_CAST(cmd));

    if (cmd->root_path) {
      gf_path_free(cmd->root_path);
      cmd->root_path = NULL;
    }
    if (cmd->conf_path) {
      gf_path_free(cmd->conf_path);
      cmd->conf_path = NULL;
    }
    if (cmd->site_path) {
      gf_path_free(cmd->site_path);
      cmd->site_path = NULL;
    }
    if (cmd->src_path) {
      gf_path_free(cmd->src_path);
      cmd->src_path = NULL;
    }

    if (GF_CMD_LIST_CAST(cmd)->site) {
      gf_site_free(GF_CMD_LIST_CAST(cmd)->site);
      GF_CMD_LIST_CAST(cmd)->site = NULL;
    }
    gf_free(cmd);
  }
}

/* -------------------------------------------------------------------------- */

static gf_status
list_process(gf_cmd_list* cmd) {
  gf_validate(cmd);
  
  return GF_SUCCESS;
}

gf_status
gf_cmd_list_execute(gf_cmd_base* cmd) {
  gf_validate(cmd);

  _(list_process(GF_CMD_LIST_CAST(cmd)));
  
  return GF_SUCCESS;
}
