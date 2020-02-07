/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_version.c
** @brief Module version.
*/

#include <libgf/gf_countof.h>
#include <libgf/gf_memory.h>
#include <libgf/gf_version.h>

#include "gf_local.h"

int
gf_version_major(void) {
  return GF_VERSION_MAJOR;
}

int
gf_version_minor(void) {
  return GF_VERSION_MINOR;
}

int
gf_version_patch(void) {
  return GF_VERSION_PATCH;
}

const char*
gf_get_version_string(void) {
  static const char version_[] = GF_VERSION_STRING;
  return version_;
}

/* -------------------------------------------------------------------------- */

struct gf_version {
  gf_command base;
};

/*!
** @brief
**
*/

enum {
  OPT_BUILD_OPTIONS,
};

static const gf_command_info info_ = {
  .base = {
    .name        = "version",
    .description = "Show version information",
    .args        = NULL,
    .create      = gf_version_new,
    .free        = gf_version_free,
    .execute     = gf_version_execute,
  },
  .options = {
    {
      .key         = OPT_BUILD_OPTIONS,
      .opt_short   = '\0',
      .opt_long    = "build-option",
      .opt_count   = 0,
      .usage       = "--build-option",
      .description = "Also prints build options.",
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

  return GF_SUCCESS;
}

static gf_status
prepare(gf_command* cmd) {
  gf_validate(cmd);

  _(gf_command_set_info(cmd, &info_));

  return GF_SUCCESS;
}

gf_status
gf_version_new(gf_command** cmd) {
  gf_status rc = 0;
  gf_command* tmp = NULL;

  _(gf_malloc((gf_ptr*)&tmp, sizeof(gf_version)));

  rc = init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    return rc;
  }
  rc = prepare(tmp);
  if (rc != GF_SUCCESS) {
    gf_version_free(tmp);
    return rc;
  }

  *cmd = tmp;
  
  return GF_SUCCESS;
}

void
gf_version_free(gf_command* cmd) {
  if (cmd) {
    gf_command_clear(cmd);
    gf_free(cmd);
  }
}

gf_status
gf_version_execute(gf_command* cmd) {
  (void)cmd;
  gf_msg("Grayfish %s", gf_get_version_string());
  return GF_SUCCESS;
}
