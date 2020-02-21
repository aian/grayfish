/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_setup.h
** @brief Initialize the system environment (private command).
*/
#ifndef LIBGF_GF_CMD_SETUP_H
#define LIBGF_GF_CMD_SETUP_H

#pragma once

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>
#include <libgf/gf_cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gf_setup gf_setup;

#define GF_SETUP_CAST(cmd) ((gf_setup*)(cmd))

/*!
** @brief Create a new setup command object.
**
** @param [out] cmd The pointer to the new setup command object
*/

extern gf_status gf_setup_new(gf_command** cmd);
extern void gf_setup_free(gf_command* cmd);

/*!
** @brief Execute the setup process.
**
** @param [in] cmd Command object
*/

extern gf_status gf_setup_execute(gf_command* cmd);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_CMD_SETUP_H */
