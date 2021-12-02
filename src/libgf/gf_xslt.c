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
  xsltStylesheetPtr template;
};

static gf_status
xslt_ctxt_init(gf_xslt* xslt) {
  gf_validate(xslt);

  xslt->template = NULL;

  return GF_SUCCESS;
}

gf_status
gf_xslt_new(gf_xslt** xslt) {
  gf_status rc = 0;
  gf_xslt* tmp = NULL;
  
  gf_validate(xslt);

  _(gf_malloc((gf_ptr* )&tmp, sizeof(*tmp)));
  rc = xslt_ctxt_init(tmp);
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

  if (xslt->template) {
    xsltFreeStylesheet(xslt->template);
    xslt->template = NULL;
  }

  return GF_SUCCESS;
}

gf_status
gf_xslt_read_template(gf_xslt* xslt, const gf_path* path) {
  xmlDocPtr tmp = NULL;
  xsltStylesheetPtr sty = NULL;
  
  gf_validate(xslt);
  gf_validate(path);

  tmp = xmlReadFile(gf_path_get_string(path), NULL, GF_XML_PARSE_OPTIONS);
  if (!tmp) {
    gf_raise(GF_E_READ,
             "Failed to read style file. (%s)", gf_path_get_string(path));
  }
  sty = xsltParseStylesheetDoc(tmp);
  // TODO: Add error handling.
  if (xslt->template) {
    _(gf_xslt_reset(xslt));
  }
  xslt->template = sty;
  
  return GF_SUCCESS;
}

gf_status
gf_xslt_file(gf_xslt* xslt, const gf_path* dst, const gf_path* src) {
//  FILE* fp = NULL;
  xmlDocPtr doc = NULL;
  xmlDocPtr res = NULL;
  
  gf_validate(xslt);
  gf_validate(!gf_path_is_empty(dst));
  gf_validate(!gf_path_is_empty(src));

  doc = xmlReadFile(gf_path_get_string(src), NULL, GF_XML_PARSE_OPTIONS);
  if (!doc) {
    gf_raise(GF_E_READ,
             "Failed to read source file. (%s)", gf_path_get_string(src));
  }
  xmlXIncludeProcessFlags(doc, XSLT_PARSE_OPTIONS);
  
  res = xsltApplyStylesheet(xslt->template, doc, NULL);
  xmlFreeDoc(doc);
  xmlFreeDoc(res);
  
  /*
  ** TODO: Output file when res is not empty.
  */
//  fp = fopen(dst, "w");
//  if (!fp) {
//    xmlFreeDoc(res);
//    gf_raise(GF_E_OPEN, "Failed to open file. (%s)", dst);
//  }
//
//  xsltSaveResultToFile(fp, res, ctxt->template);
  
  return GF_SUCCESS;
}
