/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_help.h
** @brief Main routine of Grayfish.
*/
#ifndef LIBGF_GF_CMD_HELP_H
#define LIBGF_GF_CMD_HELP_H

#pragma once

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>
#include <libgf/gf_cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gf_cmd_help gf_cmd_help;

#define GF_CMD_HELP_CAST(cmd) ((gf_cmd_help*)(cmd))

extern gf_status gf_cmd_help_new(gf_cmd_base** cmd);
extern void gf_cmd_help_free(gf_cmd_base* cmd);

/*!
** @brief Execute the 'help' process of Grayfish
**
** @param [in] cmd Command object
*/

extern gf_status gf_cmd_help_execute(gf_cmd_base* cmd);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_CMD_HELP_H */
