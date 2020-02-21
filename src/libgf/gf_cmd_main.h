/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_main.h
** @brief Main routine of Grayfish.
*/
#ifndef LIBGF_GF_CMD_MAIN_H
#define LIBGF_GF_CMD_MAIN_H

#pragma once

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>
#include <libgf/gf_cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gf_cmd_main gf_cmd_main;

#define GF_CMD_MAIN_CAST(cmd) ((gf_cmd_main*)(cmd))

extern gf_status gf_cmd_main_new(gf_cmd_base** cmd);
extern void gf_cmd_main_free(gf_cmd_base* cmd);
extern void gf_cmd_main_show_help(const gf_cmd_base* cmd);

/*!
** @brief Execute the main process of Grayfish
**
** @param [in] cmd Command object
*/

extern gf_status gf_cmd_main_execute(gf_cmd_base* cmd);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_CMD_MAIN_H */
