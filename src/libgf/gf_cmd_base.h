/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_base.h
** @brief Abstract command interface and factory.
*/
#ifndef LIBGF_GF_CMD_BASE_H
#define LIBGF_GF_CMD_BASE_H

#pragma once

#include <libgf/config.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>
#include <libgf/gf_args.h>

/*!
** @defgroup gf_command The Commmand
**
** @brief The command module.
**
** A** command** is an execution unit in the user program. 
**
*/
///@{

typedef struct gf_command gf_command;

typedef gf_status (*gf_command_create_fn)(gf_command** cmd);
typedef void (*gf_command_free_fn)(gf_command* cmd);
typedef gf_status (*gf_command_execute_fn)(gf_command* cmd);

/*!
** @brief 
*/

struct gf_command {
  char*                 name;
  char*                 description;
  gf_args*              args;
  gf_command_create_fn  create;
  gf_command_free_fn    free;
  gf_command_execute_fn execute;
};

#define GF_COMMAND_CAST(cmd) ((gf_command* )(cmd))

struct gf_command_info {
  gf_command base;
  gf_option  options[];
};

typedef struct gf_command_info gf_command_info;

/*!
** @brief Initialize the command object.
**
**
** @warning This function is expected to be called by the derived objects of the
** <code>gf_command</code> internally. Because it may cause memory leaks or
** crash the process, you should not call this function directly.
**
** @param [in, out] cmd The command object
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_command_init(gf_command* cmd);

extern gf_status gf_command_prepare(gf_command* cmd);

extern void gf_command_clear(gf_command* cmd);

/*!
** @brief Set the attributes of the commmand class.
**
** This function is typically used in the case of stting up the derived
** class.
**
** @param [out] cmd     The command object, whose attributes are set
** @param [in]  info    The template command object defined in the derived class
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_command_set_info(
  gf_command* cmd, const gf_command_info* info);

extern gf_status gf_command_set_name(gf_command* cmd, const char* name);

extern gf_status gf_command_set_description(
  gf_command* cmd, const char* description);

extern gf_status gf_command_add_options(
  gf_command* cmd, const gf_option* options);

extern gf_status gf_command_set_args(gf_command* cmd, int* argc,  char*** argv);

/*!
** @brief Inherit the command argument state from the base command object.
**
** @param [out] dst The command object, which inherit the argument state
** @param [in]  src The command object, which is inherited by the <code>dst</code>
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_command_inherit_args(gf_command* dst, const gf_command* src);

extern gf_status gf_command_consume_args(gf_command* cmd, char** str);

extern void gf_command_free(gf_command* cmd);

extern gf_status gf_command_help(const gf_command* cmd);

extern gf_status gf_command_execute(gf_command* cmd);

///@}

/*!
** @defgroup gf_command_fcatory The Command Factory
**
** @brief Command 
**
**
**
**
*/
///@{

struct gf_command_index {
  char*                name;
  gf_command_create_fn create;
};

typedef struct gf_command_index gf_command_index;

/*!
** @brief
**
*/

extern gf_status gf_command_factory_init(void);

/*!
** @brief
**
*/

extern void gf_command_factory_clean(void);

/*!
** @brief Add concrete command entry.
**
** @param [in] index The pointer to the command index array
** @param [in] size  The size of the command index array
*/

extern gf_status gf_command_factory_add_commands(
  const gf_command_index* index, gf_size_t size);

/*!
** @brief Show helps for each commands
*/

extern gf_status gf_command_factory_show_helps(void);

/*!
** @brief Generates the command object.
**
** @param [out] cmd  Generated command object
** @param [in]  name Key string to generate the concrete command
*/

extern gf_status gf_command_create(gf_command** cmd, const char* name);

///@}

#endif  /* LIBGF_GF_CMD_BASE_H */
