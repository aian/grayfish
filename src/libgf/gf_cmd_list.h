/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_list.h
** @brief Initialize the system environment (private command).
*/
#ifndef LIBGF_GF_CMD_LIST_H
#define LIBGF_GF_CMD_LIST_H

#pragma once

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>
#include <libgf/gf_cmd_base.h>

typedef struct gf_list gf_list;

#define GF_LIST_CAST(cmd) ((gf_list*)(cmd))

/*!
** @brief Create a new list command object.
**
** @param [out] cmd The pointer to the new list command object
*/

extern gf_status gf_list_new(gf_command** cmd);
extern void gf_list_free(gf_command* cmd);

/*!
** @brief Execute the list process.
**
** @param [in] cmd Command object
*/

extern gf_status gf_list_execute(gf_command* cmd);

#endif  /* LIBGF_GF_CMD_LIST_H */
