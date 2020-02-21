/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_clean.h
** @brief Initialize the system environment (private command).
*/
#ifndef LIBGF_GF_CMD_CLEAN_H
#define LIBGF_GF_CMD_CLEAN_H

#pragma once

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>
#include <libgf/gf_cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gf_clean gf_clean;

#define GF_CLEAN_CAST(cmd) ((gf_clean*)(cmd))

/*!
** @brief Create a new clean command object.
**
** @param [out] cmd The pointer to the new clean command object
*/

extern gf_status gf_clean_new(gf_cmd_base** cmd);
extern void gf_clean_free(gf_cmd_base* cmd);

/*!
** @brief Execute the clean process.
**
** @param [in] cmd Command object
*/

extern gf_status gf_clean_execute(gf_cmd_base* cmd);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_CMD_CLEAN_H */
