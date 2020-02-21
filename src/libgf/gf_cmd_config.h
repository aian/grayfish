/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_config.h
** @brief Manages the Grayfish configuration.
*/

/*!
** @defgroup gf_config The Program Configuration Module
**
** @brief The configuration manager for Grayfish.
**
*/
/// @{
#ifndef LIBGF_GF_CONFIG_H
#define LIBGF_GF_CONFIG_H

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

extern gf_status gf_config_init(void);
extern gf_status gf_config_clean(void);
extern gf_status gf_config_read_file(const gf_path* path);
extern gf_status gf_config_write_file(const gf_path* path);

/*!
** @name Setting/Getting the configuration values
**
** 
*/
/// @{

extern int gf_config_get_int(const char* key);
extern double gf_config_get_double(const char* key);
extern char* gf_config_get_string(const char* key);
extern gf_path* gf_config_get_path(const char* key);

extern gf_status gf_config_set_int(const char* key, int value);
extern gf_status gf_config_set_double(const char* key, double value);
extern gf_status gf_config_set_string(const char* key, const char* value);
extern gf_status gf_config_set_path(const char* key, const gf_path* value);

/// @}

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

#endif  /* LIBGF_GF_CONFIG_H */
/// @}
