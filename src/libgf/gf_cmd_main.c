/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_main.c
** @brief Initialize the system environment (private command).
*/
#include <assert.h>

#include <libgf/gf_countof.h>
#include <libgf/gf_memory.h>
#include <libgf/gf_string.h>
#include <libgf/gf_path.h>
#include <libgf/gf_cmd_base.h>
#include <libgf/gf_cmd_main.h>

#include "gf_local.h"

struct gf_cmd_main {
  gf_cmd_base base;
  gf_bool  version;
  gf_bool  help;
  gf_path* base_path;
};

/*!
** @brief
**
*/

enum {
  OPT_VERSION,
  OPT_HELP,
  OPT_LOG,
  OPT_DIRECTORY,
};

static const gf_cmd_base_info info_ = {
  .base = {
    .name        = "main",
    .description = "Main process",
    .args        = NULL,
    .create      = gf_cmd_main_new,
    .free        = gf_cmd_main_free,
    .execute     = gf_cmd_main_execute,
  },
  .options = {
    {
      .key         = OPT_VERSION,
      .opt_short   = 'v',
      .opt_long    = "version",
      .opt_count   = 0,
      .usage       = "-v, --version",
      .description = "Show version.",
    },
    {
      .key         = OPT_HELP,
      .opt_short   = 'h',
      .opt_long    = "help",
      .opt_count   = 0,
      .usage       = "-h, --help",
      .description = "Show help.",
    },
    {
      .key         = OPT_LOG,
      .opt_short   = 'l',
      .opt_long    = "log",
      .opt_count   = 1,
      .usage       = "-l <level>, --log=<level>",
      .description = "Set the log level.",
    },
    {
      .key         = OPT_DIRECTORY,
      .opt_short   = 'C',
      .opt_long    = "directory",
      .opt_count   = 1,
      .usage       = "-C <path>, --directory <path>",
      .description = "Change the current directory.",
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

  /* Initialize the base class */
  _(gf_cmd_base_init(cmd));
  /* Initialize the members of this class */
  GF_CMD_MAIN_CAST(cmd)->version = NULL;
  GF_CMD_MAIN_CAST(cmd)->help = NULL;
  GF_CMD_MAIN_CAST(cmd)->base_path = NULL;

  return GF_SUCCESS;
}

static gf_status
prepare(gf_cmd_base* cmd) {
  gf_validate(cmd);

  _(gf_cmd_base_set_info(cmd, &info_));

  return GF_SUCCESS;
}

gf_status
gf_cmd_main_new(gf_cmd_base** cmd) {
  gf_status rc = 0;
  gf_cmd_base* tmp = NULL;
  
  gf_validate(cmd);

  _(gf_malloc((gf_ptr* )&tmp, sizeof(gf_cmd_main)));

  rc = init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    return rc;
  }
  rc = prepare(tmp);
  if (rc != GF_SUCCESS) {
    gf_cmd_main_free(tmp);
    return rc;
  }

  *cmd = tmp;

  return GF_SUCCESS;
}

void
gf_cmd_main_free(gf_cmd_base* cmd) {
  if (cmd) {
    gf_cmd_main* main_cmd = GF_CMD_MAIN_CAST(cmd);
    /* Clear the base class members */
    (void)gf_cmd_base_clear(GF_CMD_BASE_CAST(cmd));
    /* Clear the thi object */
    main_cmd->version = GF_FALSE;
    main_cmd->help = GF_FALSE;
    /* Destroy */
    gf_free(main_cmd);
  }
}

void
gf_cmd_main_show_help(const gf_cmd_base* cmd) {
  if (cmd) {
    gf_cmd_main* tmp = GF_CMD_MAIN_CAST(cmd);
    (void)tmp;
  }
}

static gf_status
main_build_base_path(gf_cmd_main* cmd, gf_path** path) {
  char** args = NULL;
  gf_path* tmp = NULL;
  gf_size_t count = 0;

  gf_validate(cmd);
  gf_validate(path);

  if (gf_args_is_specified(cmd->base.args, OPT_DIRECTORY)) {
    _(gf_args_get_option_args(
        cmd->base.args, OPT_DIRECTORY, &args, &count));
    assert(count == 1);
    _(gf_path_new(&tmp, args[0]));
  } else {
    _(gf_path_get_current_path(&tmp));
  }

  *path = tmp;
  
  return GF_SUCCESS;
}

static gf_status
main_change_directory(gf_cmd_main* cmd) {
  gf_validate(cmd);
  gf_validate(!gf_path_is_empty(cmd->base_path));

  if (!gf_path_is_directory(cmd->base_path)) {
    gf_raise(
      GF_E_OPTION, "Moving path %s does not exist or is not a directory.",
      gf_path_get_string(cmd->base_path));
  }
  _(gf_path_change_directory(cmd->base_path));
  gf_msg("Changed directory: %s", gf_path_get_string(cmd->base_path));
  
  return GF_SUCCESS;
}

static gf_status
main_set_options(gf_cmd_main* cmd) {
  /* Parse command line arguments */
  _(gf_args_parse(cmd->base.args));
  
  cmd->version = gf_args_is_specified(cmd->base.args, OPT_VERSION);
  cmd->help    = gf_args_is_specified(cmd->base.args, OPT_HELP);
  /* Build the base path */
  _(main_build_base_path(cmd, &cmd->base_path));

  return GF_SUCCESS;
}

static gf_status
main_process_command(gf_cmd_base* main_cmd, const char* str) {
  gf_status rc = 0;
  gf_cmd_base* cmd = NULL;
  
  gf_validate(main_cmd);
  gf_validate(!gf_strnull(str));

  _(gf_cmd_base_create(&cmd, str));

  rc = gf_cmd_base_inherit_args(cmd, main_cmd);
  if (rc != GF_SUCCESS) {
    gf_cmd_base_free(cmd);
    return rc;
  }

  rc = gf_cmd_base_execute(cmd);
  gf_cmd_base_free(cmd);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  
  return GF_SUCCESS;
}

gf_status
gf_cmd_main_execute(gf_cmd_base* cmd) {
  gf_status rc = 0;
  int argc = 0;
  char* cmd_str = NULL;
  
  gf_validate(cmd);

  _(main_set_options(GF_CMD_MAIN_CAST(cmd)));

  /* Count of the command argument */
  argc = gf_args_remain(cmd->args);

  /* Change directory */
  assert(GF_CMD_MAIN_CAST(cmd)->base_path);
  if (gf_args_is_specified(cmd->args, OPT_DIRECTORY)) {
    _(main_change_directory(GF_CMD_MAIN_CAST(cmd)));
  }
  
  if (GF_CMD_MAIN_CAST(cmd)->version) {
    /* Process version option */
    _(main_process_command(cmd, "version"));
  } else if (GF_CMD_MAIN_CAST(cmd)->help) {
    /* Process help option */
    _(main_process_command(cmd, "help"));
  } else if (argc > 0) {
    /* Process the subcommand */
    _(gf_args_consume(cmd->args, &cmd_str));
    rc = main_process_command(cmd, cmd_str);
    gf_free(cmd_str);
    if (rc != GF_SUCCESS) {
      return rc;
    }
  } else {
    /* If no subcommand has been specified, it shows a help */
    _(main_process_command(cmd, "help"));
  }

  return GF_SUCCESS;
}
