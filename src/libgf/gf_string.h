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
** @param [out] dst The pointer to the pointer which points to the duplicated string
** @param [in]  src The source string terminated with zero
** @return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_strdup(char** dst, const char* src);

/*!
** @brief Assign the duplicated string.
**
** @param [out] dst The pointer to the pointer which points to the duplicated string
** @param [in]  src The source string terminated with zero
** @return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_strassign(char** dst, const char* src);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_STRING_H */
