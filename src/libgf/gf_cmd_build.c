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
#include <libgf/gf_xslt.h>
#include <libgf/gf_cmd_build.h>

#include "gf_local.h"

struct gf_cmd_build {
  gf_cmd_base base;
  gf_site*    site;
  gf_xslt*    xslt;
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
  GF_CMD_BUILD_CAST(cmd)->xslt = NULL;

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
    if (GF_CMD_BUILD_CAST(cmd)->xslt) {
      gf_xslt_free(GF_CMD_BUILD_CAST(cmd)->xslt);
      GF_CMD_BUILD_CAST(cmd)->site = NULL;
    }

    gf_free(cmd);
  }
}

static gf_status
build_prepare_output_path(gf_cmd_build* cmd) {
  gf_validate(cmd);
  _(gf_path_evacuate(GF_CMD_BASE_CAST(cmd)->dst_path));
  return GF_SUCCESS;
}

static gf_status
build_create_directory(const gf_path* path, gf_entry* entry) {
  gf_status rc = 0;
  gf_path* current_path = NULL;
  const gf_char* file_name = NULL;
  gf_size_t cnt = 0;
  
  gf_validate(path);
  gf_validate(entry);

  file_name = gf_entry_get_file_name_string(entry);
  if (!file_name) {
    gf_raise(GF_E_STATE, "File name does not exist.");
  }
  rc = gf_path_append_string(&current_path, path, file_name);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  rc = gf_path_create_directory(current_path);
  if (rc != GF_SUCCESS) {
    gf_path_free(current_path);
    gf_throw(rc);
  }

  cnt = gf_entry_count_children(entry);
  for (gf_size_t i = 0; i < cnt; i++) {
    gf_entry* child = NULL;

    rc = gf_entry_get_child(entry, i, &child);
    if (rc != GF_SUCCESS) {
      gf_path_free(current_path);
      gf_throw(rc);
    }
    rc = build_create_directory(current_path, child);
    if (rc != GF_SUCCESS) {
      gf_path_free(current_path);
      gf_throw(rc);
    }
  }

  gf_path_free(current_path);

  return GF_SUCCESS;
}

static gf_status
build_create_directories(gf_cmd_build* cmd) {
  gf_status rc = 0;
  gf_entry *entry = NULL;
  gf_size_t cnt = 0;
  
  gf_validate(cmd);

  _(gf_site_get_root_entry(cmd->site, &entry));
  if (!entry) {
    gf_raise(GF_E_STATE, "The root entry was not found.");
  }
  // The root directory must exist. We process the children.
  for (gf_size_t i = 0; i < cnt; i++) {
    gf_entry* child = NULL;

    rc = gf_entry_get_child(entry, i, &child);
    if (rc != GF_SUCCESS) {
      gf_throw(rc);
    }
    rc = build_create_directory(GF_CMD_BASE_CAST(cmd)->dst_path, child);
    if (rc != GF_SUCCESS) {
      gf_throw(rc);
    }
  }

  return GF_SUCCESS;
}

static gf_status
build_copy_static_files(gf_cmd_build* cmd) {
  gf_validate(cmd);
  return GF_SUCCESS;
}

static gf_status
build_convert_document_files(gf_cmd_build* cmd) {
  gf_validate(cmd);
  return GF_SUCCESS;
}

static gf_status
build_process(gf_cmd_build* cmd) {
  gf_status rc = 0;
  
  gf_validate(cmd);

  /* read the site file */
  assert(!cmd->site);
  rc = gf_site_read_file(&cmd->site, GF_CMD_BASE_CAST(cmd)->site_path);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  /* prepare the output root path */
  rc = build_prepare_output_path(cmd);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  /* create directories */
  rc = build_create_directories(cmd);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  /* copy static files */
  rc = build_copy_static_files(cmd);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  /* tranlate XML files */
  rc = build_convert_document_files(cmd);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  
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
