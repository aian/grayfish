/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_local.h
** @brief Local defiinitions and declarations.
*/
#ifndef LIBGF_GF_LOCAL_H
#define LIBGF_GF_LOCAL_H

#pragma once

#include <libxml/parser.h>

#include <libgf/config.h>
#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>

/*!
** @brief Shortcut for the macro gf_throw().
*/

#define _ gf_throw

#ifndef GF_CONFIG_DIRECTORY
#define GF_CONFIG_DIRECTORY ".gf"
#endif  /* GF_CONFIG_DIRECTORY */

#ifndef GF_CONFIG_FILE_NAME
#define GF_CONFIG_FILE_NAME "gf.conf"
#endif  /* GF_CONFIG_FILE_NAME */

#ifndef GF_SITE_FILE_NAME
#define GF_SITE_FILE_NAME "site.xml"
#endif  /* GF_SITE_FILE_NAME */

/*!
** @brief The parser options for LibXML2
*/

#ifndef GF_XML_PARSE_OPTIONS
#define GF_XML_PARSE_OPTIONS \
  XML_PARSE_RECOVER   |      \
  XML_PARSE_NOERROR   |      \
  XML_PARSE_NOWARNING |      \
  XML_PARSE_NOBLANKS  |      \
  XML_PARSE_XINCLUDE
#endif

#endif  /* LIBGF_GF_LOCAL_H */
