/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_help.h
** @brief Main routine of Grayfish.
*/
#ifndef LIBGF_GF_HELP_H
#define LIBGF_GF_HELP_H

#pragma once

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>
#include <libgf/gf_command.h>

typedef struct gf_help gf_help;

#define GF_HELP_CAST(cmd) ((gf_help*)(cmd))

extern gf_status gf_help_new(gf_command** cmd);
extern void gf_help_free(gf_command* cmd);

/*!
** @brief Execute the 'help' process of Grayfish
**
** @param [in] cmd Command object
*/

extern gf_status gf_help_execute(gf_command* cmd);

#endif  /* LIBGF_GF_HELP_H */
