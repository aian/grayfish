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
  gf_site*   site;
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
list_read_site_file(gf_cmd_list* cmd) {
  if (!gf_path_file_exists(GF_CMD_BASE_CAST(cmd)->site_path)) {
    gf_raise(GF_E_STATE, "Here is not a project directory. (%s)",
             gf_path_get_string(GF_CMD_BASE_CAST(cmd)->site_path));
  }
  _(gf_site_read_file(&cmd->site, GF_CMD_BASE_CAST(cmd)->site_path));

  return GF_SUCCESS;
}

static void
list_print_entry(gf_site_node* node, const char* name) {
  (void)node;
  gf_msg("%-32s", name);
}

static gf_status
list_print_node(gf_site_node* node, int depth) {
  if (gf_site_node_is_root(node)) {
    /* List header */
  } else {
    /* Print this node  */
    if (gf_site_node_is_directory(node)) {
      char* name = NULL;
      _(gf_site_node_get_file_name(&name, node));
      /*
      ** The file name beginning with '_' will be ignored.
      */
      if (name && name[0] != '_') {
        list_print_entry(node, name);
      }
    }
  }
  /* Process children */
  if (gf_site_node_is_root(node)) {
    gf_site_node* n = gf_site_node_child(node);
    while (!gf_site_node_is_null(n)) {
      _(list_print_node(n, depth + 1));
      gf_site_node_next(n);
    }
  }
  
  return GF_SUCCESS;
}

static gf_status
list_process(gf_cmd_list* cmd) {
  gf_status rc = 0;
  gf_site_node* node = NULL;

  gf_validate(cmd);

  /* Read the existing site file */
  _(list_read_site_file(cmd));

  _(gf_site_get_root(&node, cmd->site));
  /* Print */
  rc = list_print_node(node, 0);
  gf_site_node_free(node);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  
  return GF_SUCCESS;
}

gf_status
gf_cmd_list_execute(gf_cmd_base* cmd) {
  gf_validate(cmd);

  _(list_process(GF_CMD_LIST_CAST(cmd)));
  
  return GF_SUCCESS;
}
