/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_global.h
** @brief Library global settings.
*/
#ifndef LIBGF_GF_GLOBAL_H
#define LIBGF_GF_GLOBAL_H

#pragma once

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>

#ifdef __cplusplus
extern "C" {
#endif

extern gf_status gf_global_init(void);
extern gf_status gf_global_clean(void);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_GLOBAL_H */
