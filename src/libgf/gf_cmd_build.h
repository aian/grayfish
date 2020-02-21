/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_build.h
** @brief Initialize the system environment (private command).
*/
#ifndef LIBGF_GF_CMD_BUILD_H
#define LIBGF_GF_CMD_BUILD_H

#pragma once

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>
#include <libgf/gf_cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gf_build gf_build;

#define GF_BUILD_CAST(cmd) ((gf_build*)(cmd))

/*!
** @brief Create a new build command object.
**
** @param [out] cmd The pointer to the new build command object
*/

extern gf_status gf_build_new(gf_cmd_base** cmd);
extern void gf_build_free(gf_cmd_base* cmd);

/*!
** @brief Execute the build process.
**
** @param [in] cmd Command object
*/

extern gf_status gf_build_execute(gf_cmd_base* cmd);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_CMD_BUILD_H */
