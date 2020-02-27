/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_config.c
** @brief Manages the Grayfish configuration.
*/
#include <assert.h>

#include <libxml/tree.h>

#include <libgf/gf_countof.h>
#include <libgf/gf_memory.h>
#include <libgf/gf_string.h>
#include <libgf/gf_config.h>

#include <libgf/gf_cmd_config.h>

#include "gf_local.h"

typedef struct gf_cmd_config gf_cmd_config;

struct gf_cmd_config {
  gf_cmd_base base;
};

/*!
** @brief
**
*/

enum {
  OPT_WRITE,
};

static const gf_cmd_base_info info_ = {
  .base = {
    .name        = "config",
    .description = "Edit system configrations",
    .args        = NULL,
    .create      = gf_cmd_config_new,
    .free        = gf_cmd_config_free,
    .execute     = gf_cmd_config_execute,
  },
  .options = {
    {
      .key         = OPT_WRITE,
      .opt_short   = 'w',
      .opt_long    = "write",
      .opt_count   = 1,
      .usage       = "-w <path>, --write=<path>",
      .description = "Write the current config to the specified file.",
    },
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

  return GF_SUCCESS;
}

static gf_status
prepare(gf_cmd_base* cmd) {
  gf_validate(cmd);

  _(gf_cmd_base_set_info(cmd, &info_));

  return GF_SUCCESS;
}

gf_status
gf_cmd_config_new(gf_cmd_base** cmd) {
  gf_status rc = 0;
  gf_cmd_base* tmp = NULL;

  _(gf_malloc((gf_ptr*)&tmp, sizeof(gf_cmd_config)));

  rc = init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    return rc;
  }
  rc = prepare(tmp);
  if (rc != GF_SUCCESS) {
    gf_cmd_config_free(tmp);
    return rc;
  }

  *cmd = tmp;
  
  return GF_SUCCESS;
}

void
gf_cmd_config_free(gf_cmd_base* cmd) {
  gf_cmd_config* config = NULL;

  if (cmd) {
    /* Clear the base class */
    gf_cmd_base_clear(cmd);
    /* Clear and deallocate this class */
    config = GF_CMD_CONFIG_CAST(cmd);
    // do nothing to clear for now.
    gf_free(config);
  }
}

static gf_status
config_write_file(gf_cmd_base* cmd) {
  gf_status rc = 0;
  char** opt = NULL;
  gf_path* path = NULL;
  gf_size_t cnt = 0;

  _(gf_args_get_option_args(cmd->args, OPT_WRITE, &opt, &cnt));
  if (cnt < 1) {
    gf_raise(GF_E_STATE, "Too few options for writing config file.");
  }
  _(gf_path_new(&path, opt[0]));
  rc = gf_config_write_file(path);
  gf_path_free(path);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  
  return GF_SUCCESS;
}

gf_status
gf_cmd_config_execute(gf_cmd_base* cmd) {

  _(gf_args_parse(cmd->args));

  if (gf_args_is_specified(cmd->args, OPT_WRITE)) {
    _(config_write_file(cmd));
  } else {
    _(gf_config_show());
  }

  return GF_SUCCESS;
}
