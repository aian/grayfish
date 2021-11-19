/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_build.c
** @brief Module build.
*/
#include <string.h>

#include <libgf/gf_countof.h>
#include <libgf/gf_memory.h>
#include <libgf/gf_string.h>
#include <libgf/gf_path.h>
#include <libgf/gf_cmd_config.h>
#include <libgf/gf_site.h>
#include <libgf/gf_system.h>
#include <libgf/gf_convert.h>
#include <libgf/gf_cmd_build.h>

#include "gf_local.h"

struct gf_cmd_build {
  gf_cmd_base      base;
  gf_site*         site;
  gf_convert_ctxt* ctxt;
};

/*!
** @brief
**
*/

enum {
  OPT_BUILD_OPTIONS,
};

static const gf_cmd_base_info info_ = {
  .base = {
    .name        = "build",
    .description = "Build the static website",
    .args        = NULL,
    .create      = gf_cmd_build_new,
    .free        = gf_cmd_build_free,
    .execute     = gf_cmd_build_execute,
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
  
  GF_CMD_BUILD_CAST(cmd)->site = NULL;
  GF_CMD_BUILD_CAST(cmd)->ctxt = NULL;

  return GF_SUCCESS;
}

static gf_status
prepare(gf_cmd_base* cmd) {
  gf_validate(cmd);

  _(gf_cmd_base_set_info(cmd, &info_));

  return GF_SUCCESS;
}

gf_status
gf_cmd_build_new(gf_cmd_base** cmd) {
  gf_status rc = 0;
  gf_cmd_base* tmp = NULL;

  _(gf_malloc((gf_ptr*)&tmp, sizeof(gf_cmd_build)));

  rc = init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    return rc;
  }
  rc = prepare(tmp);
  if (rc != GF_SUCCESS) {
    gf_cmd_build_free(tmp);
    return rc;
  }

  *cmd = tmp;
  
  return GF_SUCCESS;
}

void
gf_cmd_build_free(gf_cmd_base* cmd) {
  if (cmd) {
    /* Clear the base class */
    gf_cmd_base_clear(cmd);
    /* Clear and deallocate this class */
    if (GF_CMD_BUILD_CAST(cmd)->site) {
      gf_site_free(GF_CMD_BUILD_CAST(cmd)->site);
      GF_CMD_BUILD_CAST(cmd)->site = NULL;
    }
    if (GF_CMD_BUILD_CAST(cmd)->ctxt) {
      gf_convert_ctxt_free(GF_CMD_BUILD_CAST(cmd)->ctxt);
      GF_CMD_BUILD_CAST(cmd)->site = NULL;
    }

    gf_free(cmd);
  }
}

static gf_status
build_process(gf_cmd_build* cmd) {
  gf_validate(cmd);

  return GF_SUCCESS;
}

gf_status
gf_cmd_build_execute(gf_cmd_base* cmd) {
  gf_validate(cmd);

  gf_msg("Compiling documents ...");

  _(build_process(GF_CMD_BUILD_CAST(cmd)));
  
  gf_msg("Done.");
  
  return GF_SUCCESS;
}
