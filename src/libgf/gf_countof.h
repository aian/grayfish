/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_countof.h
** @brief Count elements of the specified array.
*/
#ifndef LIBGF_GF_COUNTOF_H
#define LIBGF_GF_COUNTOF_H

#pragma once

#include <libgf/config.h>

/*!
** @brief Count the elements of the specified array.
**
** @param [in] ary The array object
**
** @return The count of elements of the array
*/

#define gf_countof(ary) (sizeof(ary) / sizeof(*ary))

#endif  /* LIBGF_GF_COUNTOF_H */
