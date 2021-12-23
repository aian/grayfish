/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_help.h
** @brief Main routine of Grayfish.
*/
#include <stdio.h>

#include <libgf/gf_countof.h>
#include <libgf/gf_memory.h>
#include <libgf/gf_cmd_help.h>

#include "gf_local.h"

struct gf_cmd_help {
  gf_cmd_base base;
};


/*!
** @brief
**
*/

enum {
  OPT_ALL,
};

static const gf_cmd_base_info info_ = {
  .base = {
    .name        = "help",
    .description = "Show help",
    .args        = NULL,
    .create      = gf_cmd_help_new,
    .free        = gf_cmd_help_free,
    .execute     = gf_cmd_help_execute,
  },
  .options = {
    {
      .key         = OPT_ALL,
      .opt_short   = 'a',
      .opt_long    = "all",
      .opt_count   = 0,
      .usage       = "-a, --all",
      .description = "Show all available commands.",
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
gf_cmd_help_new(gf_cmd_base** cmd) {
  gf_status rc = 0;
  gf_cmd_base* tmp = NULL;

  _(gf_malloc((gf_ptr*)&tmp, sizeof(gf_cmd_help)));

  rc = init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    return rc;
  }
  rc = prepare(tmp);
  if (rc != GF_SUCCESS) {
    gf_cmd_help_free(tmp);
    return rc;
  }

  *cmd = tmp;
  
  return GF_SUCCESS;
}

void
gf_cmd_help_free(gf_cmd_base* cmd) {
  if (cmd) {
    gf_cmd_base_clear(cmd);
    gf_free(cmd);
  }
}

gf_status
gf_cmd_help_execute(gf_cmd_base* cmd) {
  (void)cmd;
  
  gf_msg("usage: gf [--version][--help] <command> [<args>]");
  gf_msg("");
  gf_msg("Options:");
  gf_msg("");
  gf_msg("    %-24s %s", "-v, --version", "Show version infomation");
  gf_msg("    %-24s %s", "-h, --help",    "Show help");
  gf_msg("");
  gf_msg("There are the Grayfish commands:");
  gf_msg("");
 
  _(gf_cmd_factory_show_helps());

  gf_msg("");
  gf_msg("For the help of the specific command, use 'gf <command> -h'.");

  return GF_SUCCESS;
}
