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
#include <libgf/gf_path.h>
#include <libgf/gf_args.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
** @defgroup gf_cmd_base The Commmand
**
** @brief The command module.
**
** A** command** is an execution unit in the user program. 
**
*/
///@{

typedef struct gf_cmd_base gf_cmd_base;

typedef gf_status (*gf_cmd_create_fn)(gf_cmd_base** cmd);
typedef void (*gf_cmd_free_fn)(gf_cmd_base* cmd);
typedef gf_status (*gf_cmd_execute_fn)(gf_cmd_base* cmd);

/*!
** @brief The base class of the command classes
**
*/

struct gf_cmd_base {
  char*             name;        ///< The name specified in the command line
  char*             description; ///< The command description shown by help etc.
  gf_args*          args;        ///< The args specified in the command line
  gf_path*          root_path;   ///< The root of the project directory
  gf_path*          conf_path;   ///< The directory in which config file located
  gf_path*          conf_file;   ///< The local config file path 
  gf_path*          site_path;   ///< The path of the `site file'
  gf_path*          build_path;  ///< The path of the intermediate files.
  gf_path*          src_path;    ///< The root path of the source files
  gf_path*          dst_path;    ///< The root path of the destination files
  gf_cmd_create_fn  create;      ///< The function to create this object
  gf_cmd_free_fn    free;        ///< The function to deallocate this object
  gf_cmd_execute_fn execute;     ///< The function to execute command
};

#define GF_CMD_BASE_CAST(cmd) ((gf_cmd_base* )(cmd))

struct gf_cmd_base_info {
  gf_cmd_base base;
  gf_option   options[];
};

typedef struct gf_cmd_base_info gf_cmd_base_info;

/*!
** @brief Initialize the command object.
**
**
** @warning This function is expected to be called by the derived objects of the
** <code>gf_cmd_base</code> internally. Because it may cause memory leaks or
** crash the process, you should not call this function directly.
**
** @param [in, out] cmd The command object
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_cmd_base_init(gf_cmd_base* cmd);

extern gf_status gf_cmd_base_prepare(gf_cmd_base* cmd);

extern void gf_cmd_base_clear(gf_cmd_base* cmd);

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

extern gf_status gf_cmd_base_set_info(
  gf_cmd_base* cmd, const gf_cmd_base_info* info);

extern gf_status gf_cmd_base_set_name(gf_cmd_base* cmd, const char* name);

extern gf_status gf_cmd_base_set_description(
  gf_cmd_base* cmd, const char* description);

extern gf_status gf_cmd_base_add_options(
  gf_cmd_base* cmd, const gf_option* options);

extern gf_status gf_cmd_base_set_args(gf_cmd_base* cmd, int* argc,  char*** argv);

/*!
** @brief Inherit the command argument state from the base command object.
**
** @param [out] dst The command object, which inherit the argument state
** @param [in]  src The command object, which is inherited by the <code>dst</code>
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise
*/

extern gf_status gf_cmd_base_inherit_args(gf_cmd_base* dst, const gf_cmd_base* src);

extern gf_status gf_cmd_base_consume_args(gf_cmd_base* cmd, char** str);

extern void gf_cmd_base_free(gf_cmd_base* cmd);

extern gf_status gf_cmd_base_help(const gf_cmd_base* cmd);

extern gf_status gf_cmd_base_execute(gf_cmd_base* cmd);

extern gf_bool gf_cmd_base_is_root_project_directory(const gf_cmd_base* cmd);

///@}

/*!
** @defgroup gf_cmd_fcatory The Command Factory
**
** @brief Command 
**
**
**
**
*/
///@{

struct gf_cmd_index {
  char*            name;
  gf_cmd_create_fn create;
};

typedef struct gf_cmd_index gf_cmd_index;

/*!
** @brief
**
*/

extern gf_status gf_cmd_factory_init(void);

/*!
** @brief
**
*/

extern void gf_cmd_factory_clean(void);

/*!
** @brief Add concrete command entry.
**
** @param [in] index The pointer to the command index array
** @param [in] size  The size of the command index array
*/

extern gf_status gf_cmd_factory_add_commands(
  const gf_cmd_index* index, gf_size_t size);

/*!
** @brief Show helps for each commands
*/

extern gf_status gf_cmd_factory_show_helps(void);

/*!
** @brief Generates the command object.
**
** @param [out] cmd  Generated command object
** @param [in]  name The key string to generate the concrete command
*/

extern gf_status gf_cmd_create(gf_cmd_base** cmd, const char* name);

///@}

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_CMD_BASE_H */
