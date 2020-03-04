/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_array.h
** @brief Variable array module.
*/
#ifndef LIBGF_GF_ARRAY_H
#define LIBGF_GF_ARRAY_H

#pragma once

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
** @brief Variable array type
**
** This is a opaque datatype. To instantiate, use gf_array_new().
*/

typedef struct gf_array gf_array;

typedef void (*gf_array_free_fn)(gf_any* any);

typedef gf_status (*gf_array_copy_fn)(gf_any* dst, const gf_any* src);

/*!
** @brief Create a new array object
**
** @param [out] ary The array object to be created
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_array_new(gf_array** ary);

/*!
** @brief Destroy the specified array object
**
** @param [in] ary The array object to be destroyed
*/

extern void gf_array_free(gf_array* ary);

/*!
** @brief Clear the array object
**
** This function clean up the whole elements of the specfied object. If the
** callback function gf_array_free_fn has been set, it calls this function when
** each object is to be removed.
**
** This function just update the internal counter of elements, and does not
** shrink the size of internal buffer (so it does not affect the memory
** usage. To shrink the internal buffer size, call gf_array_resize() instead.
**
** @param [in] ary The array object to be cleared
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_array_clear(gf_array* ary);

/*
** @brief Swap the internal contents of the two array objects.
**
** @param [in, out] lhs The array object to be swapped
** @param [in, out] rhs The counter part of the array object to be swapped
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_array_swap(gf_array* lhs, gf_array* rhs);

/*
** @brief Set the deallocateion callback function
**
** @param [in, out] ary The array object
** @param [in]      fn  The user defined callback to be set
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_array_set_free_fn(gf_array* ary, gf_array_free_fn fn);

/*
** @brief Set the copy callback function
**
** @param [in, out] ary The array object
** @param [in]      fn  The user defined callback to be set
**
** @return GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_array_set_copy_fn(gf_array* ary, gf_array_copy_fn fn);

extern gf_status gf_array_resize(gf_array* ary, gf_size_t size);

extern gf_status gf_array_add(gf_array* ary, gf_any value);

extern gf_status gf_array_set(gf_array* ary, gf_size_t index, gf_any value);

extern gf_status gf_array_get(const gf_array* ary, gf_size_t index, gf_any* value);

extern gf_status gf_array_remove(gf_array* ary, gf_size_t index);

extern gf_size_t gf_array_size(const gf_array* ary);

extern gf_size_t gf_array_buffer_size(const gf_array* ary);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_ARRAY_H */
