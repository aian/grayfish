/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_update.h
** @brief Initialize the system environment (private command).
*/
#ifndef LIBGF_GF_CMD_UPDATE_H
#define LIBGF_GF_CMD_UPDATE_H

#pragma once

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>
#include <libgf/gf_command.h>

typedef struct gf_update gf_update;

#define GF_UPDATE_CAST(cmd) ((gf_update*)(cmd))

/*!
** @brief Create a new update command object.
**
** @param [out] cmd The pointer to the new update command object
*/

extern gf_status gf_update_new(gf_command** cmd);
extern void gf_update_free(gf_command* cmd);

/*!
** @brief Execute the update process.
**
** @param [in] cmd Command object
*/

extern gf_status gf_update_execute(gf_command* cmd);

#endif  /* LIBGF_GF_CMD_UPDATE_H */
