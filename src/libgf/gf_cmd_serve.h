/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_serve.h
** @brief Initialize the system environment (private command).
*/
#ifndef LIBGF_GF_CMD_SERVE_H
#define LIBGF_GF_CMD_SERVE_H

#pragma once

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>
#include <libgf/gf_cmd_base.h>

typedef struct gf_serve gf_serve;

#define GF_SERVE_CAST(cmd) ((gf_serve*)(cmd))

/*!
** @brief Create a new serve command object.
**
** @param [out] cmd The pointer to the new serve command object
*/

extern gf_status gf_serve_new(gf_command** cmd);
extern void gf_serve_free(gf_command* cmd);

/*!
** @brief Execute the serve process.
**
** @param [in] cmd Command object
*/

extern gf_status gf_serve_execute(gf_command* cmd);

#endif  /* LIBGF_GF_CMD_SERVE_H */
