/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_path.h
** @brief Path management.
*/
#ifndef LIBGF_GF_PATH_H
#define LIBGF_GF_PATH_H

#pragma once

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>

#define GF_PATH_SEPARATOR_WINDOWS "\\"
#define GF_PATH_SEPARATOR_UNIX    "/"

#define GF_PATH_SEPARATOR      GF_PATH_SEPARATOR_WINDOWS
#define GF_PATH_SEPARATOR_CHAR (GF_PATH_SEPARATOR [0])

#define GF_PATH_SYSTEM_DIR ".gf"


///@{ @name The Path Object

typedef struct gf_path gf_path;

/*!
** @brief Create a path object.
**
** @param [out] path     The path object to be created
** @param [in]  path_str A path string which is set to path object
** @return GF_SUCCESS on success, GF_E_* on failure.
*/

extern gf_status gf_path_new(gf_path** path, const char* path_str);

/*!
** @brief Destroy the path object.
**
** @param [in] path The path object to be deallocated
** @return GF_SUCCESS on success, GF_E_* on failure.
*/

extern void gf_path_free(gf_path* path);

/*!
** @brief Set a path string to path object.
*/

extern gf_status gf_path_set_string(gf_path* path, const char* path_str);

/*!
** @brief Get a path string from path object.
**
** @param [in] path The path object
** @return A string array of path object.
*/

extern const char* gf_path_get_string(const gf_path* path);

/*!
** @brief Swap the members of the two path objects.
**
** @param [in, out] lhs The path object, which is swap to <code>rhs</code>
** @param [in, out] rhs The path object, which is swap to <code>lhs</code>
**
** @return Return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_path_swap(gf_path* lhs, gf_path* rhs);


extern gf_status gf_path_copy(gf_path* dst, const gf_path* src);

extern gf_status gf_path_clone(gf_path** dst, const gf_path* src);

extern gf_status gf_path_canonicalize(gf_path* path);

extern gf_status gf_path_absolute_path(gf_path* path);

/*!
**
**
*/

extern gf_bool gf_path_is_empty(const gf_path* path);

/*!
** @brief Return GF_TRUE if path exists.
**
** @param [in] path A path object to be tested if that exists
** @return GF_TRUE if the path is directory, GF_FALSE otherwise.
*/

extern gf_bool gf_path_file_exists(const gf_path* path);

/*!
** @brief Return GF_TRUE if path is directory.
**
** @param [in] path A path object to be tested if that is directory
** @return GF_TRUE if the path exists, GF_FALSE otherwise.
*/

extern gf_bool gf_path_is_directory(const gf_path* path);

/*!
** @brief Return GF_TRUE if the path has a separator at the last of path string.
**
** @param [in, out] path A path object
** @return GF_TRUE if path has a separator, GF_FALSE otherwise
*/

extern gf_bool gf_path_has_separator(const gf_path* path);

/*!
** @brief Append src into path
**
** @param [in, out] path A path object
** @param [in]      src  A path string 
*/

extern gf_status gf_path_append(gf_path* path, const gf_path* src);

/*!
** @brief Append str into path
**
** @param [out] dst A path object
** @param [in]  src A path object
** @param [in]  str A path string 
*/

extern gf_status gf_path_append_string(
  gf_path** dst, const gf_path* src, const char* str);

/*!
** @brief 
**
*/

extern gf_status gf_path_evacuate(const gf_path* path);

extern gf_status gf_path_change_directory(const gf_path* path);

/*!
** @brief 
**
*/

extern gf_status gf_path_create_directory(const gf_path* path);

/*!
** @brief 
**
**
**
*/

extern gf_status gf_path_get_module_file_path(gf_path** path);

/*!
** @brief 
**
**
**
*/

extern gf_status gf_path_get_module_directory_path(gf_path** path);

/*!
** @brief 
**
**
**
*/

extern gf_status gf_path_get_style_path(gf_path** path);

/*!
** @brief
**
**
*/

extern gf_status gf_path_get_current_path(gf_path** path);

///@}

/* -------------------------------------------------------------------------- */
/*!
** @defgroup gf_uri URI module
**
*/

/*!
** @brief The data type for the URI structure
*/

typedef struct gf_uri gf_uri;

/*!
** @brief Create the new path component object.
**
** @param [out] uri The pointer to the new URI object
** @param [in]  len The length of the each components
**
** @return Return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_uri_new(gf_uri** uri, gf_size_t len);

/*!
** @brief Destroy the path component object.
**
** @param [in] uri The pointer to the URI object to destroy
**
** @return Return GF_SUCCESS on success, GF_E_* otherwise
*/

extern void gf_uri_free(gf_uri* uri);

/*!
** @brief Parse and split into URI component
**
** @param [out] uri  The pointer to the new URI object
** @param [in]  path The string to split
** @param [in]  len  The length of the URI components
**
** @return Return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_uri_split(gf_uri** uri, const char* path, gf_size_t len);

/*!
** @brief Build the directory path from a URI object
**
** @param [out] path The created path object
** @param [in]  uri  The URI object
**
** @return Return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_uri_build_directory_path(gf_path** path, gf_uri* uri);

extern gf_bool gf_uri_match_extension(const char* path, const char* ext);

#endif  /* LIBGF_GF_PATH_H */
