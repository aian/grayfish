/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_xslt.c
** @brief Abstract API to xslt files.
*/
#include <stdlib.h>

#include <libxml/xmlmemory.h>
#include <libxml/debugXML.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlIO.h>
#include <libxml/xinclude.h>
#include <libxml/catalog.h>

#include <libxslt/xslt.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

#include <libexslt/exslt.h>

#include <libgf/gf_memory.h>
#include <libgf/gf_string.h>
#include <libgf/gf_xslt.h>

#include "gf_local.h"

/* -------------------------------------------------------------------------- */

struct gf_xslt_tuple {
  gf_char* key;
  gf_char* value;
};

gf_status
xslt_tuple_init(gf_xslt_tuple* tuple) {
  gf_validate(tuple);

  tuple->key = NULL;
  tuple->value = NULL;
  
  return GF_SUCCESS;
}

void
gf_xslt_tuple_clear(gf_xslt_tuple* tuple) {
  if (tuple) {
    if (tuple->key) {
      gf_free(tuple->key);
      tuple->key = NULL;
    }
    if (tuple->value) {
      gf_free(tuple->value);
      tuple->value = NULL;
    }
  }
}

const gf_char*
gf_xslt_tuple_get_key(gf_xslt_tuple* tuple) {
  return tuple ? tuple->key : NULL;
}

const gf_char*
gf_xslt_tuple_get_value(gf_xslt_tuple* tuple) {
  return tuple ? tuple->value : NULL;
}

gf_bool
xslt_tuple_is_null(const gf_xslt_tuple* tuple) {
  return (!tuple || !tuple->key || !tuple->value) ? GF_TRUE : GF_FALSE;
}

gf_status
gf_xslt_tuple_assign(
  gf_xslt_tuple* tuple, const gf_char* key, const gf_char* value) {
  gf_status rc = 0;
  gf_char* tmp_key = NULL;
  gf_char* tmp_value = NULL;

  gf_validate(tuple);
  gf_validate(key);
  gf_validate(value);
  
  rc = gf_strdup(&tmp_key, key);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  rc = gf_strdup(&tmp_value, value);
  if (rc != GF_SUCCESS) {
    gf_free(tmp_key);
    gf_throw(rc);
  }
  /* assign */
  if (tuple->key) {
    gf_free(tuple->key);
    tuple->key = NULL;
  }
  tuple->key = tmp_key;

  if (tuple->value) {
    gf_free(tuple->value);
    tuple->value = NULL;
  }
  tuple->value = tmp_value;
  
  return GF_SUCCESS;
}

/* -------------------------------------------------------------------------- */

#ifndef GF_XSLT_PARAM_MAX
#define GF_XSLT_PARAM_MAX 16
#endif

struct gf_xslt_param {
  gf_xslt_tuple item[GF_XSLT_PARAM_MAX + 1];
};

#define LIBXSLT_PARAM_ARRAY_CAST(ParamPtr) ((gf_char**)(ParamPtr))

gf_status
xslt_param_init(gf_xslt_param* param) {
  static const gf_size_t cnt = GF_XSLT_PARAM_MAX + 1;
  
  gf_validate(param);

  for (gf_size_t i = 0; i < cnt; i++) {
    _(xslt_tuple_init(&param->item[i]));
  }
  
  return GF_SUCCESS;
}

gf_status
gf_xslt_param_new(gf_xslt_param** param) {
  gf_status rc = 0;
  gf_xslt_param* tmp = NULL;
  
  gf_validate(param);

  _(gf_malloc((gf_ptr*)&tmp, sizeof(*tmp)));
  rc = xslt_param_init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    gf_throw(rc);
  }
  *param = tmp;
  
  return GF_SUCCESS;
}

void
gf_xslt_param_free(gf_xslt_param* param) {
  if (param) {
    for (gf_size_t i = 0; i <= GF_XSLT_PARAM_MAX; i++) {
      gf_xslt_tuple_clear(&param->item[i]);
    }
    gf_free(param);
  }
}

static gf_bool
xslt_param_is_full(const gf_xslt_param* param) {
  gf_size_t cnt = 0;

  if (!param) {
    assert(0);
    return GF_TRUE;
  }
  cnt = gf_xslt_param_count(param);
  
  return cnt < GF_XSLT_PARAM_MAX ? GF_FALSE : GF_TRUE;
}

gf_status
gf_xslt_param_set_value(
  gf_xslt_param* param, const gf_char* key, const gf_char* value) {
  gf_validate(param);
  gf_validate(!gf_strnull(key));
  gf_validate(!gf_strnull(value));  
  
  return GF_SUCCESS;
}

gf_status
gf_xslt_param_add_tuple(
  gf_xslt_param* param, const gf_xslt_tuple* tuple) {
  gf_validate(param);
  gf_validate(tuple);

  if (xslt_param_is_full(param)) {
    gf_raise(GF_E_PARAM, "XSLT param is full-tank.");
  }

  return GF_SUCCESS;
}

gf_size_t
gf_xslt_param_count(const gf_xslt_param* param) {
  gf_size_t cnt = 0;

  for (cnt = 0; cnt <= GF_XSLT_PARAM_MAX; cnt++) {
    if (xslt_tuple_is_null(&param->item[cnt])) {
      break;
    }
  }

  return cnt;
}

gf_status
gf_xslt_param_get_tuple(
  const gf_xslt_param* param, gf_size_t index, const gf_xslt_tuple** tuple) {
  gf_validate(param);
  gf_validate(index < GF_XSLT_PARAM_MAX);
  gf_validate(tuple);

  *tuple = &param->item[index];
  
  return GF_SUCCESS;
}

gf_status
gf_xslt_param_get_value(
  const gf_xslt_param* param, const gf_char* key, const gf_char** value) {
  gf_validate(param);
  gf_validate(!gf_strnull(key));
  gf_validate(!value);

  for (gf_size_t i = 0; i < GF_XSLT_PARAM_MAX; i++) {
    if (xslt_tuple_is_null(&param->item[i])) {
      *value = NULL;
      break;
    }
    if (!strcmp(param->item[i].key, key)) {
      *value = param->item[i].value;
    }
  }

  return GF_SUCCESS;
}

/* -------------------------------------------------------------------------- */

/*!
**
*/

struct gf_xslt {
  xsltStylesheetPtr xsl;
  xmlDocPtr         res;  ///< Result XML tree
};

static gf_status
xslt_init(gf_xslt* xslt) {
  gf_validate(xslt);

  xslt->xsl = NULL;
  xslt->res = NULL;

  return GF_SUCCESS;
}

gf_status
gf_xslt_new(gf_xslt** xslt) {
  gf_status rc = 0;
  gf_xslt* tmp = NULL;
  
  gf_validate(xslt);

  _(gf_malloc((gf_ptr* )&tmp, sizeof(*tmp)));
  rc = xslt_init(tmp);
  if (rc != GF_SUCCESS) {
    gf_xslt_free(tmp);
    gf_throw(rc);
  }

  *xslt = tmp;

  return GF_SUCCESS;
}

void
gf_xslt_free(gf_xslt* xslt) {
  if (xslt) {
    (void)gf_xslt_reset(xslt);
    gf_free(xslt);
  }
}

gf_status
gf_xslt_reset(gf_xslt* xslt) {
  gf_validate(xslt);

  if (xslt->xsl) {
    xsltFreeStylesheet(xslt->xsl);
    xslt->xsl = NULL;
  }

  return GF_SUCCESS;
}

gf_status
gf_xslt_read_template(gf_xslt* xslt, const gf_path* path) {
  xmlDocPtr tmp = NULL;
  xsltStylesheetPtr xsl = NULL;
  
  gf_validate(xslt);
  gf_validate(path);

  tmp = xmlReadFile(gf_path_get_string(path), NULL, GF_XML_PARSE_OPTIONS);
  if (!tmp) {
    gf_raise(GF_E_READ,
             "Failed to read style file. (%s)", gf_path_get_string(path));
  }
  xsl = xsltParseStylesheetDoc(tmp);
  // TODO: Add error handling.
  if (xslt->xsl) {
    _(gf_xslt_reset(xslt));
  }
  xslt->xsl = xsl;
  
  return GF_SUCCESS;
}

static gf_status
xslt_release_result(gf_xslt* xslt) {
  gf_validate(xslt);
  
  if (xslt->res) {
    xmlFreeDoc(xslt->res);
    xslt->res = NULL;
  }
  
  return GF_SUCCESS;
}

gf_status
gf_xslt_process(gf_xslt* xslt, const gf_path* path) {
  gf_status rc = 0;
  xmlDocPtr doc = NULL;
  xmlDocPtr res = NULL;
  
  gf_validate(xslt);
  gf_validate(!gf_path_is_empty(path));

  doc = xmlReadFile(gf_path_get_string(path), NULL, GF_XML_PARSE_OPTIONS);
  if (!doc) {
    gf_raise(GF_E_READ,
             "Failed to read source file. (%s)", gf_path_get_string(path));
  }
  xmlXIncludeProcessFlags(doc, XSLT_PARSE_OPTIONS);

  res = xsltApplyStylesheet(xslt->xsl, doc, NULL);
  xmlFreeDoc(doc);
  if (!res) {
    gf_raise(GF_E_API,
             "Failed to transform the file. (%s)", gf_path_get_string(path));
  }
  rc = xslt_release_result(xslt);
  if (rc != GF_SUCCESS) {
    gf_throw(rc);
  }
  xslt->res = res;
    
  return GF_SUCCESS;
}


gf_status
gf_xslt_write_file(gf_xslt* xslt, const gf_path* path) {
  int ret = 0;
  FILE* fp = NULL;

  fp = fopen(gf_path_get_string(path), "w");
  if (!fp) {
    gf_raise(GF_E_OPEN, "Failed to open file. (%s)", gf_path_get_string(path));
  }
  
  ret = xsltSaveResultToFile(fp, xslt->res, xslt->xsl);
  fclose(fp);
  if (ret < 0) {
    gf_raise(GF_E_OPEN, "Failed to save file. (%s)", gf_path_get_string(path));
  }

  return GF_SUCCESS;
}
