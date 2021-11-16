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

typedef struct gf_author gf_author;

extern gf_status gf_author_new(gf_author** author);
extern void gf_author_free(gf_author* author);
extern gf_status gf_author_set_name(gf_author* author, const gf_string* name);
extern gf_status gf_author_set_mail(gf_author* author, const gf_string* mail);

/* -------------------------------------------------------------------------- */

enum gf_object_type {
  GF_OBJECT_TYPE_UNKNOWN = 0,
  GF_OBJECT_TYPE_SITE    = 1,
  GF_OBJECT_TYPE_SECTION = 2,
  GF_OBJECT_TYPE_ENTRY   = 3,
  GF_OBJECT_TYPE_FILE    = 4,
};

typedef enum gf_object_type gf_object_type;


typedef struct gf_object gf_object;

extern gf_status gf_object_init(gf_object* obj);
extern gf_status gf_object_clear(gf_object* obj);
extern gf_status gf_object_set_type(gf_object* obj, gf_object_type type);
extern gf_status gf_object_set_title(gf_object* obj, const gf_string* title);
extern gf_status gf_object_set_author(gf_object* obj, const gf_author* author);
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
** @brief Traverse the specifed directory tree and collect information.
**
** @param [in, out] site      The pointer to the site object
** @param [in]      root_path The start point for traversing the files
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_site_update(gf_site* site, const gf_path* root_path);

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

/* -------------------------------------------------------------------------- */

/*!
** @brief The pointer of the site document node.
**
*/

typedef struct gf_site_node gf_site_node;

extern gf_status gf_site_get_root(gf_site_node** node, gf_site* site);

extern void gf_site_node_free(gf_site_node* node);

extern gf_bool gf_site_node_is_null(const gf_site_node* node);

extern gf_site_node* gf_site_node_next(gf_site_node* node);

extern gf_site_node* gf_site_node_prev(gf_site_node* node);

extern gf_site_node* gf_site_node_child(gf_site_node* node);

extern gf_site_node* gf_site_node_parent(gf_site_node* node);

extern gf_bool gf_site_node_is_root(const gf_site_node* node);

extern gf_bool gf_site_node_is_file(const gf_site_node* node);

extern gf_bool gf_site_node_is_directory(const gf_site_node* node);

extern gf_status gf_site_node_get_full_path(
  gf_path** full_path, const gf_site_node* node);

extern gf_status gf_site_node_get_tree_path(
  gf_path** tree_path, const gf_site_node* node);

extern gf_status gf_site_node_get_file_name(
  char** file_name, const gf_site_node* node);

/* -------------------------------------------------------------------------- */

typedef struct gf_doc_info gf_doc_info;

extern void gf_doc_info_free(gf_doc_info* info);
extern gf_status gf_doc_info_get(gf_doc_info** info, const gf_site_node* node);
extern const char* gf_doc_info_title(const gf_doc_info* info);
extern const char* gf_doc_info_path(const gf_doc_info* info);
extern const char* gf_doc_info_date(const gf_doc_info* info);
extern const char* gf_doc_info_status(const gf_doc_info* info);
extern gf_status gf_doc_info_print(FILE* fp, const gf_site_node* node);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_SITE_H */
