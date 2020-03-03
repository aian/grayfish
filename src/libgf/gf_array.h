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

typedef struct gf_array gf_array;

extern gf_status gf_array_new(gf_array** ary);

extern void gf_array_free(gf_array* ary);



#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_ARRAY_H */
