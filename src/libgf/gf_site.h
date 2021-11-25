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
#include <libgf/gf_datetime.h>
#include <libgf/gf_file_info.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

/*!
** @brief Types of an gf_entry object
*/

enum gf_entry_type {
  GF_ENTRY_TYPE_UNKNOWN  = 0,  ///< Unknown object type
  GF_ENTRY_TYPE_SITE     = 1,  ///< The site root object (site.gf)
  GF_ENTRY_TYPE_SECTION  = 2,  ///< The section object (meta.gf)
  GF_ENTRY_TYPE_DOCUMENT = 3,  ///< The entry object (index.dbk)
  GF_ENTRY_TYPE_PROC     = 4,  ///< The custom proecess object (proc.gf)
  GF_ENTRY_TYPE_FILE     = 5,  ///< Ordinary files
};

typedef enum gf_entry_type gf_entry_type;

/*!
** @brief State of an gf_entry object
*/

enum gf_entry_state {
  GF_ENTRY_STATE_UNKNOWN   = 0,
  GF_ENTRY_STATE_DRAFT     = 1,
  GF_ENTRY_STATE_PUBLISHED = 2,
};

typedef enum gf_entry_state gf_entry_state;

/* -------------------------------------------------------------------------- */

/*!
** @brief A category object.
*/

typedef struct gf_category gf_category;

/*!
** @brief Creates a new category object.
*/

extern gf_status gf_category_new(gf_category** cat);

/*!
** @brief Destroy a category object.
*/

extern void gf_category_free(gf_category* cat);

/*!
** @brief Sets a ID string to a category object.
*/

extern gf_status gf_category_set_id(gf_category* cat, const gf_string* id);

/*!
** @brief Sets a name string to a category object.
*/

extern gf_status gf_category_set_name(gf_category* cat, const gf_string* name);

/* -------------------------------------------------------------------------- */

/*!
** @brief A site entry object.
*/

typedef struct gf_entry gf_entry;

/*!
** @brief Creates a new entry object.
*/

extern gf_status gf_entry_new(gf_entry** entry);

/*!
** @brief Destroy a new entry object.
*/

extern void gf_entry_free(gf_entry* entry);

/*!
** @brief Sets A site entry object.
*/

extern gf_status gf_entry_set_file_info(gf_entry* entry, gf_file_info* info);

/* -------------------------------------------------------------------------- */

typedef struct gf_site gf_site;

/*!
** @brief Create a new site object.
**
** This function creates a new gf_site object. 
**
** @param [out] site The pointer to the pointer, that points to the new site.
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_site_new(gf_site** site);

/*!
** @brief Traverse the specifed directory tree and collect site information.
**
** @param [out] site The pointer to the site object
** @param [in]  path The start point for traversing the files
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_site_scan(gf_site** site, const gf_path* path);

/*!
** @brief Destruct a site object.
**
** @param [in] site The pointer to the site object to be destroyed.
*/

extern void gf_site_free(gf_site* site);

/*!
** @brief Reset the state of a site object
**
** @param [in] site The pointer to the site object to be reset.
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_site_reset(gf_site* site);

/*!
** @brief Write directory information to specified file.
**
** @param [in, out] site The pointer to the site object
** @param [in]      path The file path to be written
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_site_write_file(const gf_site* site, const gf_path* path);

/*!
** @brief Write directory information to specified file.
**
** @param [in, out] site The pointer to the site object
** @param [in]      path The file path to be written
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_site_read_file(gf_site** site, const gf_path* path);


#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_SITE_H */
