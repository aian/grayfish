/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_site.h
** @brief Site the document file information.
*/
#ifndef LIBGF_GF_SITE_H
#define LIBGF_GF_SITE_H

#pragma once

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>
#include <libgf/gf_path.h>

#include <libgf/gf_string.h>
#include <libgf/gf_date.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

enum gf_object_type {
  GF_OBJECT_TYPE_UNKNOWN = 0,  ///< Unknown object type
  GF_OBJECT_TYPE_SITE    = 1,  ///< The site root object (site.gf)
  GF_OBJECT_TYPE_SECTION = 2,  ///< The section object (meta.gf)
  GF_OBJECT_TYPE_ENTRY   = 3,  ///< The entry object (index.dbk)
  GF_OBJECT_TYPE_PROC    = 4,  ///< The custom proecess object (proc.gf)
  GF_OBJECT_TYPE_FILE    = 5,  ///< Ordinary files
};
typedef enum gf_object_type gf_object_type;

enum gf_object_state {
  GF_OBJECT_STATE_UNKNOWN   = 0,
  GF_OBJECT_STATE_DRAFT     = 1,
  GF_OBJECT_STATE_PUBLISHED = 2,
};
typedef enum gf_object_state gf_object_state;

typedef struct gf_object gf_object;

extern gf_status gf_object_init(gf_object* obj);
extern gf_status gf_object_clear(gf_object* obj);
extern gf_status gf_object_set_type(gf_object* obj, gf_object_type type);
extern gf_status gf_object_set_title(gf_object* obj, const gf_string* title);
extern gf_status gf_object_set_author(gf_object* obj, const gf_string* author);
extern gf_status gf_object_set_update_date(gf_object* obj, const gf_date* date);
extern gf_status gf_object_set_create_date(gf_object* obj, const gf_date* date);

/* -------------------------------------------------------------------------- */

typedef struct gf_site gf_site;

/*!
** @brief Create the new site.
**
** @param [out] site The pointer to the pointer, that points to the new site.
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_site_new(gf_site** site);

/*!
** @brief Destruct the site.
**
** @param [in] site The pointer to the site object to be destroyed.
*/

extern void gf_site_free(gf_site* site);

/*!
** @brief Reset the state of the site
**
** @param [in] site The pointer to the site object to be reset.
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_site_reset(gf_site* site);

/*!
** @brief Traverse the specifed directory tree and collect site information.
**
** @param [out] site The pointer to the site object
** @param [in]  path The start point for traversing the files
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_site_scan(gf_site** site, const gf_path* path);

/*!
** @brief Write the directory information to the specified file.
**
** @param [in, out] site The pointer to the site object
** @param [in]      path The file path to be written
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_site_write_file(const gf_site* site, const gf_path* path);

/*!
** @brief Write the directory information to the specified file.
**
** @param [in, out] site The pointer to the site object
** @param [in]      path The file path to be written
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_site_read_file(gf_site** site, const gf_path* path);


#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_SITE_H */
