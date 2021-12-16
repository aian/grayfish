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
#include <libgf/gf_shell.h>
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
  if (!gf_path_file_exists(GF_CMD_BASE_CAST(cmd)->build_path)) {
    _(gf_shell_make_directory(GF_CMD_BASE_CAST(cmd)->build_path));
  }
  
  return GF_SUCCESS;
}

static gf_status
build_create_directory(const gf_path* path, gf_entry* entry) {
  gf_status rc = 0;
  gf_path* local_path = NULL;
  gf_size_t cnt = 0;
  
  gf_validate(path);
  gf_validate(entry);
  
  local_path = gf_entry_get_local_path(entry, path);
  if (!local_path) {
    gf_raise(GF_E_STATE, "Failed to build a path.");
  }
  rc = gf_path_append(local_path, GF_PATH_PARENT);
  if (rc != GF_SUCCESS) {
    gf_path_free(local_path);
    gf_throw(rc);
  }
  rc = gf_path_absolute_path(local_path);
  if (rc != GF_SUCCESS) {
    gf_path_free(local_path);
    gf_throw(rc);
  }
  rc = gf_path_create_directory(local_path);
  gf_path_free(local_path);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }

  cnt = gf_entry_count_children(entry);
  for (gf_size_t i = 0; i < cnt; i++) {
    gf_entry* child = NULL;

    rc = gf_entry_get_child(entry, i, &child);
    if (rc != GF_SUCCESS) {
      gf_throw(rc);
    }
    rc = build_create_directory(path, child);
    if (rc != GF_SUCCESS) {
      gf_throw(rc);
    }
  }

  return GF_SUCCESS;
}

static gf_status
build_create_directory_set(gf_cmd_build* cmd) {
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
build_get_static_path(
  gf_path** static_path, const gf_entry* entry, const gf_path* root) {
  gf_status rc = 0;
  gf_path* static_file = NULL;
  gf_path* path = NULL;
  
  gf_validate(static_path);
  gf_validate(entry);
  gf_validate(root);

  path = gf_entry_get_local_path(entry, root);
  if (!path) {
    gf_raise(GF_E_PATH, "Failed to build a path string.");
  }
  rc = gf_path_new(&static_file, "/../_");
  if (rc != GF_SUCCESS) {
    gf_path_free(path);
    gf_throw(rc);
  }
  rc = gf_path_append(path, static_file);
  gf_path_free(static_file);
  if (rc != GF_SUCCESS) {
    gf_path_free(path);
    gf_throw(rc);
  }
  rc = gf_path_absolute_path(path);
  if (rc != GF_SUCCESS) {
    gf_path_free(path);
    gf_throw(rc);
  }

  *static_path = path;

  return GF_SUCCESS;
}

static gf_status
build_copy_static_file(
  gf_entry* entry, const gf_path* src, const gf_path* dst) {
  gf_status rc = 0;
  gf_size_t cnt = 0;
  gf_path* src_path = NULL;
  gf_path* dst_path = NULL;

  rc = build_get_static_path(&src_path, entry, src);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  rc = build_get_static_path(&dst_path, entry, dst);
  if (rc != GF_SUCCESS) {
    gf_path_free(src_path);
    gf_throw(rc);
  }
  rc = gf_shell_copy_tree(dst_path, src_path);
  gf_path_free(src_path);
  gf_path_free(dst_path);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  /* Process children */
  cnt = gf_entry_count_children(entry);
  for (gf_size_t i = 0; i < cnt; i++) {
    gf_entry* child = NULL;

    rc = gf_entry_get_child(entry, i, &child);
    if (rc != GF_SUCCESS) {
      gf_throw(rc);
    }
    rc = build_copy_static_file(child, src, dst);
    if (rc != GF_SUCCESS) {
      gf_throw(rc);
    }
  }

  return GF_SUCCESS;
}

static gf_status
build_copy_static_file_set(gf_cmd_build* cmd) {
  gf_status rc = 0;
  gf_entry* entry = NULL;
  /* alias */
  const gf_path* src = GF_CMD_BASE_CAST(cmd)->src_path;
  const gf_path* dst = GF_CMD_BASE_CAST(cmd)->dst_path;;

  gf_validate(cmd);

  rc = gf_site_get_root_entry(cmd->site, &entry);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  rc = build_copy_static_file(entry, src, dst);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  
  return GF_SUCCESS;
}

static xmlNodePtr
build_xml_get_process_set(xmlNodePtr node) {
  if (node) {
    if (!xmlStrcmp(node->name, BAD_CAST "process-set")) {
      return node;
    }
    for (xmlNodePtr cur = node->children; cur; cur = cur->next) {
      return build_xml_get_process_set(cur);
    }
  }
  return NULL;
}

static gf_status
build_get_style_path(
  gf_path** style_path, const gf_char* method, const gf_path* root) {
  gf_status rc = 0;
  gf_path* tmp = NULL;
  gf_char buf[1024] = { 0 };

  gf_validate(style_path);
  gf_validate(!gf_strnull(method));
  gf_validate(root);

  // TODO: check the length of the string 'method'.
  
  sprintf_s(buf, 1024, "%s.xsl", method);
  rc = gf_path_append_string(&tmp, root, buf);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  
  *style_path = tmp;

  return GF_SUCCESS;
}

static gf_status
build_process_site_file_low(xmlNodePtr node, gf_cmd_build* cmd) {
  gf_status rc = 0;
  gf_xslt* xslt = NULL;
  gf_path* style_path = NULL;
  
  const gf_char* method = NULL;
  const gf_char* output = NULL;
  
  gf_validate(node);
  gf_validate(cmd);

  method = (gf_char*)xmlGetProp(node, BAD_CAST "method");
  output = (gf_char*)xmlGetProp(node, BAD_CAST "output");

  if (!method) {
    gf_raise(GF_E_READ, "Invalid meta file.");
  }

  /* Prepare an XSLT processor */
  rc = gf_xslt_new(&xslt);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  rc = build_get_style_path(
    &style_path, method, GF_CMD_BASE_CAST(cmd)->style_path);
  if (rc != GF_SUCCESS) {
    gf_xslt_free(xslt);
    gf_throw(rc);
  }
  rc = gf_xslt_read_template(xslt, style_path);
  gf_path_free(style_path);
  if (rc != GF_SUCCESS) {
    gf_xslt_free(xslt);
    gf_throw(rc);
  }
  rc = gf_xslt_set_param(
    xslt, "conf-file", gf_path_get_string(GF_CMD_BASE_CAST(cmd)->conf_path));
  if (rc != GF_SUCCESS) {
    gf_xslt_free(xslt);
    gf_throw(rc);
  }
  rc = gf_xslt_set_param(
    xslt, "site-file", gf_path_get_string(GF_CMD_BASE_CAST(cmd)->site_path));
  if (rc != GF_SUCCESS) {
    gf_xslt_free(xslt);
    gf_throw(rc);
  }
  /* XSLT process */
  rc = gf_xslt_process(xslt, GF_CMD_BASE_CAST(cmd)->site_path);
  if (rc != GF_SUCCESS) {
    gf_xslt_free(xslt);
    gf_throw(rc);
  }
  /* Output when it is needed */
  if (!gf_strnull(output)) {
    gf_path* output_path = NULL;

    rc = gf_path_new(&output_path, output);
    if (rc != GF_SUCCESS) {
      gf_xslt_free(xslt);
      gf_throw(rc);
    }
    rc = gf_xslt_write_file(xslt, output_path);
    if (rc != GF_SUCCESS) {
      gf_xslt_free(xslt);
      gf_throw(rc);
    }
  }

  gf_xslt_free(xslt);
  
  return GF_SUCCESS;
}

static gf_status
build_process_site_file(gf_entry* entry, gf_cmd_build* cmd) {
  gf_status rc = 0;
  gf_size_t cnt = 0;

  gf_validate(entry);
  gf_validate(cmd);

  if (gf_entry_is_section(entry)) {
    gf_path* path = NULL;
    xmlDocPtr doc = NULL;
    xmlNodePtr root = NULL;
    xmlNodePtr node = NULL;

    const gf_path* src = NULL;

    /* alias */
    src = GF_CMD_BASE_CAST(cmd)->src_path;
    
    /* Read meta.gf */
    path = gf_entry_get_local_path(entry, src);
    if (!path) {
      gf_raise(GF_E_PATH, "Failed to build a path.");
    }
    doc = xmlReadFile(gf_path_get_string(path), NULL, GF_XML_PARSE_OPTIONS);
    gf_path_free(path);
    if (!doc) {
      gf_raise(GF_E_PARSE, "Failed to read site file");
    }
    root = xmlDocGetRootElement(doc);
    if (root) {
      node = build_xml_get_process_set(root);
      if (node) {
        for (xmlNodePtr cur = node->children; cur; cur = cur->next) {
          assert(!xmlStrcmp(cur->name, BAD_CAST "process"));
          _(build_process_site_file_low(cur, cmd));
        }
      }
    }
  }
  cnt = gf_entry_count_children(entry);
  for (gf_size_t i = 0; i < cnt; i++) {
    gf_entry* child = NULL;

    rc = gf_entry_get_child(entry, i, &child);
    if (rc != GF_SUCCESS) {
      gf_throw(rc);
    }
    rc = build_process_site_file(child, cmd);
    if (rc != GF_SUCCESS) {
      gf_throw(rc);
    }
  }
  
  return GF_SUCCESS;
}

static gf_status
build_get_document_stylesheet(
  gf_xslt** xslt, const gf_path* style_root, const gf_path* src_path) {
  gf_status rc = 0;
  xmlDocPtr doc = NULL;
  xmlNodePtr root = NULL;
  xmlChar* role = NULL;
  gf_char* method = NULL;
  gf_char style_file[1024] = { 0 };
  gf_path* style_path = NULL;
  gf_xslt* tmp = NULL;
  
  gf_validate(xslt);
  gf_validate(src_path);

  doc = xmlReadFile(gf_path_get_string(src_path), NULL, GF_XML_PARSE_OPTIONS);
  if (!doc) {
    gf_raise(GF_E_PARSE, "Failed to read a document file.");
  }
  root = xmlDocGetRootElement(doc);
  if (!root) {
    gf_raise(GF_E_PARSE, "Failed to read a document file.");
  }
  role = xmlGetProp(root, BAD_CAST "role");
  if (role && role[0]) {
    method = (gf_char*)role;
  } else {
    method = (gf_char*)(root->name);
  }
  assert(method && method[0]);
  sprintf_s(style_file, 1024, "%s.xsl", method);

  rc = gf_path_append_string(&style_path, style_root, style_file);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  rc = gf_xslt_new(&tmp);
  if (rc != GF_SUCCESS) {
    gf_path_free(style_path);
    gf_throw(rc);
  }
  rc = gf_xslt_read_template(tmp, style_path);
  gf_path_free(style_path);
  if (rc != GF_SUCCESS) {
    gf_xslt_free(tmp);
    gf_throw(rc);
  }
  
  *xslt = tmp;

  return GF_SUCCESS;
}

static gf_status
build_process_document_file_low(
  const gf_path* dst, const gf_path* src, gf_cmd_build* cmd) {
  gf_status rc = 0;
  gf_xslt* xslt = NULL;
  
  gf_validate(src);
  gf_validate(dst);
  gf_validate(cmd);

  rc = build_get_document_stylesheet(
    &xslt, GF_CMD_BASE_CAST(cmd)->style_path, src);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  rc = gf_xslt_process(xslt, src);
  if (rc != GF_SUCCESS) {
    gf_xslt_free(xslt);
    gf_throw(rc);
  }

  gf_xslt_free(xslt);
  
  return GF_SUCCESS;
}

static gf_status
build_process_document_file(gf_entry* entry, gf_cmd_build* cmd) {
  gf_status rc = 0;
  gf_size_t cnt = 0;

  gf_validate(entry);
  gf_validate(cmd);
  
  if (gf_entry_is_document(entry)) {
    gf_path* src = NULL;
    gf_path* dst = NULL;

    src = gf_entry_get_local_path(entry, GF_CMD_BASE_CAST(cmd)->src_path);
    if (!src) {
      gf_raise(GF_E_PATH, "Failed to build a local document path.");
    }
    // NOTE: dst_path is a directory path
    dst = GF_CMD_BASE_CAST(cmd)->dst_path;
    if (!dst) {
      gf_path_free(src);
      gf_raise(GF_E_PATH, "Failed to build a local document path.");
    }
    rc = build_process_document_file_low(dst, src, cmd);
    gf_path_free(src);
    gf_path_free(dst);
    if (rc != GF_SUCCESS) {
      gf_throw(rc);
    }
  }
  cnt = gf_entry_count_children(entry);
  for (gf_size_t i = 0; i < cnt; i++) {
    gf_entry* child = NULL;

    rc = gf_entry_get_child(entry, i, &child);
    if (rc != GF_SUCCESS) {
      gf_throw(rc);
    }
    rc = build_process_document_file(child, cmd);
    if (rc != GF_SUCCESS) {
      gf_throw(rc);
    }
  }

  return GF_SUCCESS;
}

static gf_status
build_convert_document_file_set(gf_cmd_build* cmd) {
  gf_status rc = 0;
  gf_entry* entry = NULL;
  
  gf_validate(cmd);

  rc = gf_site_get_root_entry(cmd->site, &entry);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  /* Convert site.xml */
  rc = build_process_site_file(entry, cmd);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  /* Convert documents */
  rc = build_process_document_file(entry, cmd);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  
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
  rc = build_create_directory_set(cmd);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  /* copy static files */
  rc = build_copy_static_file_set(cmd);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  /* tranlate XML files */
  rc = build_convert_document_file_set(cmd);
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
