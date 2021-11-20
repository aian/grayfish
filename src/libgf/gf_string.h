/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_string.h
** @brief String operation.
*/
#ifndef LIBGF_GF_STRING_H
#define LIBGF_GF_STRING_H

#pragma once

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
** @brief Returns GF_TRUE when 
**
** @param [out] str String array.
** @return Boolean value whether the specifed string is null or not.
*/

extern gf_bool gf_strnull(const char* str);

/*!
** @brief Returns the length of a null terminated string array.
**
** @param [out] str String array.
** @return Length of the string
*/

extern gf_size_t gf_strlen(const char* str);

/*!
** @brief Duplicate string with a newly allocated memory space.
**
** @param [out] dst The pointer which points to the duplicated string
** @param [in]  src The source string terminated with zero
** @return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_strdup(char** dst, const char* src);

/*!
** @brief Assign the duplicated string.
**
** @param [out] dst The pointer to the string object to be assigned
** @param [in]  src The source string terminated with zero
** @return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_strassign(char** dst, const char* src);

/* -------------------------------------------------------------------------- */

/*!
** @brief Structured string object type
**
** This type is an opaque type.
*/

typedef struct gf_string gf_string;

/*!
** @brief Invalid character constant
**
** This constant represents a invalid character in the gf_string module.
*/

extern const gf_int GF_STRING_INVALID_CHAR;

/*!
** @brief Create a new structured string object
**
** @param [out] str A pointer to the new string object
**
** @return GF_SUCCESS on success, GF_E* otherwise
*/

extern gf_status gf_string_new(gf_string** str);

/*!
** @brief Destroy a structured string object
**
** @param [in] str A string object to be destroyed
*/

extern void gf_string_free(gf_string* str);

extern void gf_string_free_any(gf_any* any);

extern gf_status gf_string_set(gf_string* str, const gf_char* s);

extern const gf_char* gf_string_get(const gf_string* str);

extern gf_size_t gf_string_size(const gf_string* str);

extern gf_int gf_string_get_at(const gf_string* str, gf_size_t index);

extern gf_status gf_string_copy(gf_string* dst, const gf_string* src);

extern gf_status gf_string_clone(gf_string** dst, const gf_string* src);

extern gf_status gf_string_assign(gf_string** dst, const gf_string* src);

extern gf_bool gf_string_is_empty(const gf_string* str);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_STRING_H */
