/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_convert.c
** @brief Abstract API to convert files.
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
#include <libgf/gf_convert.h>

#include "gf_local.h"

/*!
**
*/

struct gf_convert_ctxt {
  gf_convert_type   type;
  xsltStylesheetPtr template;
};

static gf_status
convert_ctxt_init(gf_convert_ctxt* ctxt) {
  gf_validate(ctxt);

  ctxt->type = GF_CONVERT_TYPE_UNKNOWN;
  ctxt->template = NULL;

  return GF_SUCCESS;
}

gf_status
gf_convert_ctxt_new(gf_convert_ctxt** ctxt, gf_convert_type type) {
  gf_convert_ctxt* tmp = NULL;
  
  gf_validate(ctxt);

  _(gf_malloc((gf_ptr* )&tmp, sizeof(*tmp)));
  _(convert_ctxt_init(tmp));

  tmp->type = type;
  *ctxt = tmp;

  return GF_SUCCESS;
}

void
gf_convert_ctxt_free(gf_convert_ctxt* ctxt) {
  if (ctxt) {
    (void)gf_convert_ctxt_reset(ctxt);
    gf_free(ctxt);
  }
}

gf_status
gf_convert_ctxt_reset(gf_convert_ctxt* ctxt) {
  gf_validate(ctxt);

  if (ctxt->template) {
    xsltFreeStylesheet(ctxt->template);
    ctxt->template = NULL;
  }

  return GF_SUCCESS;
}

gf_status
gf_convert_ctxt_read_template(gf_convert_ctxt* ctxt, const char* path) {
  xmlDocPtr tmp = NULL;
  xsltStylesheetPtr sty = NULL;
  
  gf_validate(ctxt);
  gf_validate(path);

  tmp = xmlReadFile(path, NULL, GF_XML_PARSE_OPTIONS);
  if (!tmp) {
    gf_raise(GF_E_READ, "Failed to read style file. (%s)", path);
  }
  sty = xsltParseStylesheetDoc(tmp);
  // TODO: Add error handling.
  if (ctxt->template) {
    _(gf_convert_ctxt_reset(ctxt));
  }
  ctxt->template = sty;
  
  return GF_SUCCESS;
}

gf_status
gf_convert_file(gf_convert_ctxt* ctxt, const char* dst, const char* src) {
//  FILE* fp = NULL;
  xmlDocPtr doc = NULL;
  xmlDocPtr res = NULL;
  
  gf_validate(ctxt);
  gf_validate(!gf_strnull(dst));
  gf_validate(!gf_strnull(src));

  doc = xmlReadFile(src, NULL, GF_XML_PARSE_OPTIONS);
  if (!doc) {
    gf_raise(GF_E_READ, "Failed to read source file. (%s)", src);
  }
  xmlXIncludeProcessFlags(doc, XSLT_PARSE_OPTIONS);
  
  res = xsltApplyStylesheet(ctxt->template, doc, NULL);
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
