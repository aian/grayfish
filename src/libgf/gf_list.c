/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_list.c
** @brief Module list.
*/

#include <libgf/gf_countof.h>
#include <libgf/gf_memory.h>
#include <libgf/gf_string.h>
#include <libgf/gf_path.h>
#include <libgf/gf_config.h>
#include <libgf/gf_system.h>
#include <libgf/gf_site.h>
#include <libgf/gf_list.h>

#include "gf_local.h"

struct gf_list {
  gf_command base;
  gf_path*   root_path;
  gf_path*   conf_path;
  gf_path*   site_path;
  gf_path*   src_path;
  gf_site*   site;
};


/*!
** @brief
**
*/

static const gf_command_info info_ = {
  .base = {
    .name        = "list",
    .description = "List document status",
    .args        = NULL,
    .create      = gf_list_new,
    .free        = gf_list_free,
    .execute     = gf_list_execute,
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
init(gf_command* cmd) {
  gf_validate(cmd);

  _(gf_command_init(cmd));

  GF_LIST_CAST(cmd)->root_path = NULL;
  GF_LIST_CAST(cmd)->conf_path = NULL;
  GF_LIST_CAST(cmd)->site_path = NULL;
  GF_LIST_CAST(cmd)->src_path = NULL;
  GF_LIST_CAST(cmd)->site = NULL;

  return GF_SUCCESS;
}

/*
** NOTE: This function is almost the same as gf_update.c:update_make_paths(),
** We should refactor to call this as a common function.
*/

static gf_status
list_make_paths(gf_list* cmd) {
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
prepare(gf_command* cmd) {
  gf_validate(cmd);

  _(gf_command_set_info(cmd, &info_));
  _(list_make_paths(GF_LIST_CAST(cmd)));

  return GF_SUCCESS;
}

gf_status
gf_list_new(gf_command** cmd) {
  gf_status rc = 0;
  gf_command* tmp = NULL;

  _(gf_malloc((gf_ptr*)&tmp, sizeof(gf_list)));

  rc = init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    return rc;
  }
  rc = prepare(tmp);
  if (rc != GF_SUCCESS) {
    gf_list_free(tmp);
    return rc;
  }

  *cmd = tmp;
  
  return GF_SUCCESS;
}

void
gf_list_free(gf_command* cmd) {
  if (cmd) {
    gf_command_clear(GF_COMMAND_CAST(cmd));

    if (GF_LIST_CAST(cmd)->root_path) {
      gf_path_free(GF_LIST_CAST(cmd)->root_path);
      GF_LIST_CAST(cmd)->root_path = NULL;
    }
    if (GF_LIST_CAST(cmd)->conf_path) {
      gf_path_free(GF_LIST_CAST(cmd)->conf_path);
      GF_LIST_CAST(cmd)->conf_path = NULL;
    }
    if (GF_LIST_CAST(cmd)->site_path) {
      gf_path_free(GF_LIST_CAST(cmd)->site_path);
      GF_LIST_CAST(cmd)->site_path = NULL;
    }
    if (GF_LIST_CAST(cmd)->src_path) {
      gf_path_free(GF_LIST_CAST(cmd)->src_path);
      GF_LIST_CAST(cmd)->src_path = NULL;
    }

    if (GF_LIST_CAST(cmd)->site) {
      gf_site_free(GF_LIST_CAST(cmd)->site);
      GF_LIST_CAST(cmd)->site = NULL;
    }
    gf_free(cmd);
  }
}

/* -------------------------------------------------------------------------- */

static gf_status
list_read_site_file(gf_list* cmd) {
  if (!gf_path_file_exists(cmd->site_path)) {
    gf_raise(GF_E_STATE, "Here is not a project directory. (%s)",
             gf_path_get_string(cmd->site_path));
  }
  _(gf_site_read_file(&cmd->site, cmd->site_path));

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
list_process(gf_list* cmd) {
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
gf_list_execute(gf_command* cmd) {
  gf_validate(cmd);

  _(list_process(GF_LIST_CAST(cmd)));
  
  return GF_SUCCESS;
}
