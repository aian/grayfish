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

typedef struct gf_main gf_main;

#define GF_MAIN_CAST(cmd) ((gf_main*)(cmd))

extern gf_status gf_main_new(gf_command** cmd);
extern void gf_main_free(gf_command* cmd);
extern void gf_main_show_help(const gf_command* cmd);

/*!
** @brief Execute the main process of Grayfish
**
** @param [in] cmd Command object
*/

extern gf_status gf_main_execute(gf_command* cmd);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_CMD_MAIN_H */
