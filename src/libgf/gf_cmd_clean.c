/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_clean.c
** @brief Module clean.
*/

#include <libgf/gf_countof.h>
#include <libgf/gf_memory.h>
#include <libgf/gf_cmd_clean.h>

#include "gf_local.h"

struct gf_clean {
  gf_command base;
};


/*!
** @brief
**
*/

static const gf_command_info info_ = {
  .base = {
    .name        = "clean",
    .description = "Clean the site directory",
    .args        = NULL,
    .create      = gf_clean_new,
    .free        = gf_clean_free,
    .execute     = gf_clean_execute,
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

  return GF_SUCCESS;
}

static gf_status
prepare(gf_command* cmd) {
  gf_validate(cmd);

  _(gf_command_set_info(cmd, &info_));

  return GF_SUCCESS;
}

gf_status
gf_clean_new(gf_command** cmd) {
  gf_status rc = 0;
  gf_command* tmp = NULL;

  _(gf_malloc((gf_ptr*)&tmp, sizeof(gf_clean)));

  rc = init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    return rc;
  }
  rc = prepare(tmp);
  if (rc != GF_SUCCESS) {
    gf_clean_free(tmp);
    return rc;
  }

  *cmd = tmp;
  
  return GF_SUCCESS;
}

void
gf_clean_free(gf_command* cmd) {
  if (cmd) {
    gf_command_clear(cmd);
    gf_free(cmd);
  }
}

gf_status
gf_clean_execute(gf_command* cmd) {
  (void)cmd;
  return GF_SUCCESS;
}
