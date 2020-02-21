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
#include <libgf/gf_cmd_config.h>

#include "gf_local.h"

struct gf_cmd_config {
  gf_cmd_base base;
  xmlDocPtr  doc;
};

typedef struct gf_cmd_config gf_cmd_config;

static gf_cmd_config config_ = { 0 };

#define CONFIG_NODE_ROOT   BAD_CAST "config"
#define CONFIG_NODE_PARAM  BAD_CAST "param"
#define CONFIG_ATTR_KEY    BAD_CAST "k"
#define CONFIG_ATTR_VALUE  BAD_CAST "v"

#define X_(str) BAD_CAST str

static gf_status
config_doc_set_default_params(xmlNodePtr root) {
  /*
  ** Parameter Definitions
  */
  static const struct {
    xmlChar* key;
    xmlChar* value;
  } params[] = {
    { X_("threads"),         X_("0")                          },
    { X_("site.title"),      X_("My Awesome Website")         },
    { X_("site.author"),     X_("John Due")                   },
    { X_("site.email"),      X_("john@example.com")           },
    { X_("site.pub-path"),   X_("pub")                        },
    { X_("site.src-path"),   X_("src")                        },
    { X_("site.style-path"), X_("..\\etc\\docbook\\book.xsl") },
    { X_("site.data"),       X_("data")                       },
    { X_("http.host"),       X_("localhost")                  },
    { X_("http.port"),       X_("8080")                       },
    { X_("http.root"),       X_("/")                          },
    { X_("http.url"),        X_("example.com")                },
    { X_("remote.scp.host"), X_("example.com")                },
    { X_("remote.scp.port"), X_("22")                         },
    { X_("remote.scp.root"), X_("/")                          },
  };
  
  gf_validate(root);
  
  const gf_size_t size = sizeof(params) / sizeof(*params);
  
  for (gf_size_t i = 0; i < size; i++) {
    xmlNodePtr cur = NULL;
    xmlNodePtr ret = NULL;

    cur = xmlNewNode(NULL, CONFIG_NODE_PARAM);
    if (!cur) {
      gf_raise(GF_E_API, "Failed to create parameter node.");
    }
    xmlSetProp(cur, CONFIG_ATTR_KEY,   params[i].key);
    xmlSetProp(cur, CONFIG_ATTR_VALUE, params[i].value);

    ret = xmlAddChild(root, cur);
    if (!ret) {
      xmlFreeNode(cur);
      gf_raise(GF_E_API, "Failed to add parameter node.");
    }
  }

  return GF_SUCCESS;
}

static gf_status
config_doc_init(xmlDocPtr doc) {
  gf_status rc = 0;
  xmlNodePtr root = NULL;
  xmlNodePtr cur = NULL;
  
  gf_validate(doc);

  root = xmlNewNode(NULL, CONFIG_NODE_ROOT);
  if (!root) {
    gf_raise(GF_E_API, "Failed to create config root.");
  }
  rc = config_doc_set_default_params(root);
  if (rc != GF_SUCCESS) {
    xmlFreeNode(root);
    return rc;
  }
  cur = xmlDocSetRootElement(doc, root);
  if (cur) {
    xmlFreeNode(cur);
    cur = NULL;
  }

  return GF_SUCCESS;
}

gf_status
gf_config_init(void) {
  gf_status rc = 0;
  xmlDocPtr tmp = NULL;

  tmp = xmlNewDoc(BAD_CAST "1.0");
  if (!tmp) {
    gf_raise(GF_E_API, "Failed to config document.");
  }
  rc = config_doc_init(tmp);
  if (rc != GF_SUCCESS) {
    xmlFreeDoc(tmp);
    return rc;
  }
  if (config_.doc) {
    xmlFreeDoc(config_.doc);
    config_.doc = NULL;
  }
  config_.doc = tmp;
  
  return GF_SUCCESS;
}

gf_status
gf_config_clean(void) {
  if (config_.doc) {
    xmlFreeDoc(config_.doc);
    config_.doc = NULL;
  }
  return GF_SUCCESS;
}

static gf_status
config_find_param(xmlNodePtr* param, const xmlNodePtr node, xmlChar* key) {
  gf_validate(param);
  gf_validate(node);
  gf_validate(!gf_strnull((char *)key));

  *param = NULL;

  for (xmlNodePtr cur = node->children; cur; cur = cur->next) {
    xmlChar* attr = NULL;

    /* Process element node only */
    if (cur->type != XML_ELEMENT_NODE) {
      continue;
    }

    if (xmlStrcmp(cur->name, CONFIG_NODE_PARAM) != 0) {
      gf_warn("Invalid node for config file (%s). Ignored.", cur->name);
      continue;
    }
    attr = xmlGetProp(cur, CONFIG_ATTR_KEY);
    if (attr) {
      if (!xmlStrcmp(key, attr)) {
        *param = cur;
        return GF_SUCCESS;
      }
    }
  }

  return GF_SUCCESS;
}

static gf_status
config_merge_node(xmlNodePtr dst, const xmlNodePtr src) {
  gf_validate(dst);
  gf_validate(src);

  for (xmlNodePtr cur = dst->children; cur; cur = cur->next) {
    xmlNodePtr param = NULL;
    xmlChar* key = NULL;
    xmlChar* value = NULL;

    /* Process element node only */
    if (cur->type != XML_ELEMENT_NODE) {
      continue;
    }
    if (xmlStrcmp(cur->name, CONFIG_NODE_PARAM) != 0) {
      gf_warn("Invalid node for master config (%s). Ignored.", cur->name);
      continue;
    }
    key = xmlGetProp(cur, CONFIG_ATTR_KEY);
    if (!key) {
      gf_raise(GF_E_CONFIG, "The master config key on the param not found.");
    }
    _(config_find_param(&param, src, key));
    value = xmlGetProp(param, CONFIG_ATTR_VALUE);
    if (!value) {
      gf_warn("The value of the param '%s' not found. Ignored.", key);
      continue;
    }
    xmlSetProp(cur, CONFIG_ATTR_VALUE, value);
  }
  
  return GF_SUCCESS;
}

static gf_status
config_merge_doc(xmlDocPtr dst, const xmlDocPtr src) {
  xmlNodePtr dst_root = NULL;
  xmlNodePtr src_root = NULL;
  
  dst_root = xmlDocGetRootElement(dst);
  if (!dst_root) {
    gf_raise(GF_E_CONFIG, "Invalid master config. Root element is not found.");
  }
  if (xmlStrcmp(dst_root->name, CONFIG_NODE_ROOT) != 0) {
    gf_raise(GF_E_CONFIG, "Invalid master root element (%s)", dst_root->name);
  }
  
  src_root = xmlDocGetRootElement(src);
  if (!src_root) {
    gf_raise(GF_E_CONFIG, "Invalid config file. Root element is not found.");
  }
  if (xmlStrcmp(dst_root->name, CONFIG_NODE_ROOT) != 0) {
    gf_warn("Invalid config root element (%s). Not read.", dst_root->name);
    return GF_SUCCESS;
  }
  _(config_merge_node(dst_root, src_root));
  
  return GF_SUCCESS;
}

gf_status
gf_config_read_file(const gf_path* path) {
  gf_status rc = 0;
  xmlDocPtr doc = NULL;
  const char* s = NULL;

  gf_validate(!gf_path_is_empty(path));

  if (!gf_path_file_exists(path)) {
    gf_warn("Config file '%s' does not exist.", gf_path_get_string(path));
    return GF_SUCCESS;
  }
  s = gf_path_get_string(path);
  doc = xmlParseFile(s);
  if (!doc) {
    gf_raise(GF_E_READ, "Failed to read the configuration file (%s)", s);
  }
  /* Merge configuration */
  rc = config_merge_doc(config_.doc, doc);
  xmlFreeDoc(doc);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  
  return GF_SUCCESS;
}

gf_status
gf_config_write_file(const gf_path* path) {
  int ret = 0;
  const char* file_path = gf_path_get_string(path);
  
  gf_validate(!gf_path_is_empty(path));
  
  if (!config_.doc) {
    gf_raise(GF_E_WRITE, "Failed to write config file.");
  }
  ret = xmlSaveFormatFileEnc(file_path, config_.doc, "UTF-8", 1);
  if (ret < 0) {
    gf_raise(GF_E_WRITE, "Failed to write config file.");
  }
  
  return GF_SUCCESS;
}

static const char*
config_get_param_value(const char* key) {
  gf_status rc = 0;
  xmlNodePtr root = NULL;
  xmlNodePtr node = NULL;
  xmlChar* attr = NULL;
  
  if (gf_strnull(key)) {
    gf_warn("The param key is NULL.");
    return NULL;
  }
  root = xmlDocGetRootElement(config_.doc);
  rc = config_find_param(&node, root, X_(key));
  if (rc != GF_SUCCESS) {
    gf_warn("The param value is not found.");
    return NULL;
  }
  if (node) {
    attr = xmlGetProp(node, CONFIG_ATTR_VALUE);
  }
  
  return (const char*)attr;
}

int
gf_config_get_int(const char* key) {
  int ret = 0;
  const char* str = NULL;
  char* e = NULL;

  str = config_get_param_value(key);
  if (str) {
    ret = strtol(str, &e, 10);
    if (*e != '\0') {
      gf_warn("The param value is expected to be integer, but not (%s)", str);
      return 0;
    }
  }
  
  return ret;
}

double
gf_config_get_double(const char* key) {
  double ret = 0.;
  const char* str = NULL;
  char* e = NULL;

  str = config_get_param_value(key);
  if (str) {
    ret = strtod(str, &e);
    if (*e != '\0') {
      gf_warn("The param value is expected to be float, but not (%s)", str);
      return 0.;
    }
  }
  
  return ret;
}

char* 
gf_config_get_string(const char* key) {
  gf_status rc = 0;
  char* ret = NULL;
  const char* str = NULL;

  str = config_get_param_value(key);
  if (str) {
    rc = gf_strdup(&ret, str);
    if (rc != GF_SUCCESS) {
      return NULL;
    }
  }
  
  return ret;
}

gf_path* 
gf_config_get_path(const char* key) {
  gf_status rc = 0;
  gf_path* ret = NULL;
  const char* str = NULL;

  str = config_get_param_value(key);
  if (str) {
    rc = gf_path_new(&ret, str);
    if (rc != GF_SUCCESS) {
      return NULL;
    }
  }
  
  return ret;
}

gf_status
gf_config_set_int(const char* key, int value) {
  char buf[256] = { 0 };
  
  gf_validate(!gf_strnull(key));

  snprintf(buf, 256, "%d", value);
  _(gf_config_set_string(key, buf));

  return GF_SUCCESS;
}

gf_status
gf_config_set_double(const char* key, double value) {
  char buf[256] = { 0 };
  
  gf_validate(!gf_strnull(key));

  snprintf(buf, 256, "%f", value);
  _(gf_config_set_string(key, buf));

  return GF_SUCCESS;
}

gf_status
gf_config_set_string(const char* key, const char* value) {
  gf_status rc = 0;
  xmlNodePtr root = NULL;
  xmlNodePtr node = NULL;
  
  gf_validate(!gf_strnull(key));
  gf_validate(value);

  root = xmlDocGetRootElement(config_.doc);
  rc = config_find_param(&node, root, X_(key));
  if (rc != GF_SUCCESS || !node) {
    gf_raise(GF_E_PARAM, "The param is not found (%s).", key);
  }
  xmlSetProp(node, CONFIG_ATTR_VALUE, X_(value));
  
  return GF_SUCCESS;
}

gf_status
gf_config_set_path(const char* key, const gf_path* value) {
  _(gf_config_set_string(key, gf_path_get_string(value)));
  return GF_SUCCESS;
}

/* -------------------------------------------------------------------------- */

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

static gf_status
config_show(gf_cmd_base* cmd) {
  xmlNodePtr root = NULL;

  (void)cmd;
  
  root = xmlDocGetRootElement(config_.doc);
  if (!root || xmlStrcmp(root->name, CONFIG_NODE_ROOT) != 0) {
    gf_raise(GF_E_INTERNAL, "Invalid config structure.");
  }
  gf_msg("Current Configurations:");
  for (xmlNodePtr cur = root->children; cur; cur = cur->next) {
    xmlChar* key = NULL;
    xmlChar* value = NULL;

    key = xmlGetProp(cur, CONFIG_ATTR_KEY);
    value = xmlGetProp(cur, CONFIG_ATTR_VALUE);

    gf_msg("%-32s\t%-32s", key, value);
  }
  
  return GF_SUCCESS;
}

gf_status
gf_cmd_config_execute(gf_cmd_base* cmd) {

  _(gf_args_parse(cmd->args));

  if (gf_args_is_specified(cmd->args, OPT_WRITE)) {
    _(config_write_file(cmd));
  } else {
    _(config_show(cmd));
  }

  return GF_SUCCESS;
}
