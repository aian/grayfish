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
