/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_system.h
** @brief System management.
*/
#ifndef LIBGF_GF_SYSTEM_H
#define LIBGF_GF_SYSTEM_H

#pragma once

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>
#include <libgf/gf_path.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
** @brief 
**
**
**
*/

extern gf_bool gf_system_is_project_path(const gf_path* path);

/*!
** @brief Create a project directory
**
** @param [in] base The path where the project is to be created.
** @param [in] name The project name
**
** @return Return GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_system_make_project(const gf_path* base, const char* name);

/*!
** @brief 
**
**
**
*/

extern gf_status gf_system_get_system_config_path(gf_path** path);

/*!
** @brief 
**
**
**
*/

extern gf_status gf_system_get_system_config_file_path(gf_path** path);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_SYSTEM_H */
