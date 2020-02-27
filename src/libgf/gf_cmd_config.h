/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_config.h
** @brief Grayfish configuration command.
*/
#ifndef LIBGF_GF_CMD_CONFIG_H
#define LIBGF_GF_CMD_CONFIG_H

#pragma once

#include <libgf/config.h>

#include <stdint.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>
#include <libgf/gf_path.h>
#include <libgf/gf_cmd_base.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
** @brief Command object for the process configuration
**
*/

#define GF_CMD_CONFIG_CAST(cmd) ((gf_cmd_config*)(cmd))

extern gf_status gf_cmd_config_new(gf_cmd_base** cmd);

extern void gf_cmd_config_free(gf_cmd_base* cmd);

/*!
** @brief Execute the 'config' process of Grayfish
**
** @param [in] cmd Command object
*/

extern gf_status gf_cmd_config_execute(gf_cmd_base* cmd);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_CMD_CONFIG_H */
