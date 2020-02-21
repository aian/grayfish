/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_swap.h
** @brief Swap the two objects.
*/
#ifndef LIBGF_GF_SWAP_H
#define LIBGF_GF_SWAP_H

#pragma once

#include <libgf/config.h>

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
** @brief Return the size of two objects.
**
** @note If the size of two objects are different from each other, it returns -1
** to invoke the compile errors in <code>gf_swap()</code>.
**
** @param [in] x The object to test its size.
** @param [in] y The object to test its size.
*/

#define gf_sizeof(x, y) sizeof(x) == sizeof(y) ? (signed)sizeof(x) : -1

/*!
** @brief Swap the values of the two objects.
**
** @note The function memcpy_s() is 
** @note This macro does not check if the two object is the same type.
** 
** @param [in, out] lhs 
** @param [in, out] rhs 
*/

#define gf_swap(lhs, rhs) do {                      \
    unsigned char tmp_[gf_sizeof(lhs, rhs)];        \
    memcpy_s(tmp_, sizeof(lhs), &rhs, sizeof(lhs)); \
    memcpy_s(&rhs, sizeof(lhs), &lhs, sizeof(lhs)); \
    memcpy_s(&lhs, sizeof(lhs), tmp_, sizeof(lhs)); \
  } while (0);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_SWAP_H */
