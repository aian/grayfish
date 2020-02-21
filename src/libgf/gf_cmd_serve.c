/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_serve.c
** @brief Module serve.
*/

#include <libgf/gf_countof.h>
#include <libgf/gf_memory.h>
#include <libgf/gf_convert.h>
#include <libgf/gf_cmd_serve.h>

#include "gf_local.h"

struct gf_serve {
  gf_command   base;
  unsigned int port;
};

/*!
** @brief
**
*/

enum {
  OPT_PORT,
};

static const gf_command_info info_ = {
  .base = {
    .name        = "serve",
    .description = "Run the local server.",
    .args        = NULL,
    .create      = gf_serve_new,
    .free        = gf_serve_free,
    .execute     = gf_serve_execute,
  },
  .options = {
    {
      .key         = OPT_PORT,
      .opt_short   = 'p',
      .opt_long    = "--port",
      .opt_count   = 1,
      .usage       = "-p <port>, --port=<port>",
      .description = "Specify the port number.",
    },
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

  GF_SERVE_CAST(cmd)->port = GF_SERVE_PORT_DEFAULT;

  return GF_SUCCESS;
}

static gf_status
prepare(gf_command* cmd) {
  gf_validate(cmd);

  _(gf_command_set_info(cmd, &info_));

  return GF_SUCCESS;
}

gf_status
gf_serve_new(gf_command** cmd) {
  gf_status rc = 0;
  gf_command* tmp = NULL;

  _(gf_malloc((gf_ptr*)&tmp, sizeof(gf_serve)));

  rc = init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    return rc;
  }
  rc = prepare(tmp);
  if (rc != GF_SUCCESS) {
    gf_serve_free(tmp);
    return rc;
  }

  *cmd = tmp;
  
  return GF_SUCCESS;
}

void
gf_serve_free(gf_command* cmd) {
  if (cmd) {
    /* Clear the base class */
    gf_command_clear(cmd);
    // do nothing to clear for now.
    gf_free(cmd);
  }
}

gf_status
gf_serve_execute(gf_command* cmd) {
  (void)cmd;
  return GF_SUCCESS;
}
