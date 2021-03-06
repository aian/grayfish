/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_version.h
** @brief Module version.
*/
#ifndef LIBGF_GF_CMD_VERSION_H
#define LIBGF_GF_CMD_VERSION_H

#pragma once

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>
#include <libgf/gf_cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int gf_cmd_version_major(void);
extern int gf_cmd_version_minor(void);
extern int gf_cmd_version_patch(void);

extern const char* gf_get_version_string(void);


typedef struct gf_cmd_version gf_cmd_version;

#define GF_CMD_VERSION_CAST(cmd) ((gf_cmd_version*)(cmd))

extern gf_status gf_cmd_version_new(gf_cmd_base** cmd);
extern void gf_cmd_version_free(gf_cmd_base* cmd);
extern void gf_cmd_version_show_help(const gf_cmd_base* cmd);

/*!
** @brief Execute the 'version' process of Grayfish
**
** @param [in] cmd Command object
*/

extern gf_status gf_cmd_version_execute(gf_cmd_base* cmd);

#ifdef __cplusplus
}
#endif

#endif /* LIBGF_GF_CMD_VERSION_H */
