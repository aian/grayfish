/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_args.h
** @brief Operations for command arguments.
*/

/*!
** @defgroup gf_args The Command Line Arguments
**
** @brief The helper module for managing the command line arguments.
**
** ## Overview
**
** This module manages the specified command line arguments as the option
** parameters for Grayfish. 
**
** ## Usage
**
** ### 1. Create the <code>gf_args</code> object
**
** @code
**   gf_status rc = 0;
**   gf_args* args = NULL;
**
**   rc = gf_args_new(&args);
**   if (rc != GF_SUCCESS) {
**     return rc;
**   }
** @endcode
**
** ### 2. Register the command options
**
** @code
**   gf_throw(gf_args_add(args, 'h', "help", 0, "Show help."));
**   gf_throw(gf_args_add(args, 'v', "version", 0, "Show version."));
**   gf_throw(gf_args_add(args, 'C', "config", 2, "Edit the config file."));
** @endcode
**
** ### 3. Parse the command argument vector
**
** @code
**   void
**   parse(int argc, char* argv[]) {
**     gf_throw(gf_args_parse(args, &argc, &argv));
**   }
** @endcode
**
*/
#ifndef LIBGF_GF_ARGS_H
#define LIBGF_GF_ARGS_H

#pragma once

#include <libgf/config.h>

#include <stdint.h>

#include <libgf/gf_datatype.h>
#include <libgf/gf_error.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gf_args gf_args;
typedef struct gf_args_entry gf_args_entry;

/*!
** @brief 
*/

struct gf_option {
  int     key;          ///<
  char    opt_short;    ///<
  char*   opt_long;     ///<
  uint8_t opt_count;    ///<
  char*   usage;        ///<
  char*   description;  ///<
};

typedef struct gf_option gf_option;

#define GF_OPTION_NULL    \
  {                       \
    .key         = 0,     \
    .opt_short   = 0,     \
    .opt_long    = NULL,  \
    .opt_count   = 0,     \
    .usage       = NULL,  \
    .description = NULL,  \
  }

/* -------------------------------------------------------------------------- */

/*!
** @brief Create a new argument object
**
** @param [out] args The newly created argument object
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_args_new(gf_args** args);

/*!
** @brief Destoy a sepecified argument object
**
** @param [in] args The pointer to the argument object to destroy
*/

extern void gf_args_free(gf_args* args);

/*!
** @brief Add a command argument entry
**
** @param [in, out] args   The argument object
** @param [in]      option The argument option
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_args_add_option(gf_args* args, const gf_option* option);

/*!
** @brief Add command arguments by option table
**
** @param [in, out] args    The argument object
** @param [in]      options The option definition table
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_args_add_option_table(
  gf_args* args, const gf_option* options);

/*!
** @brief Parse the command arguments.
**
** @param [in, out] args The argument object
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_args_parse(gf_args* args);

/*!
** @brief Set the command line arguments into the argument object.
**
** @param [in, out] args The argument object
** @param [in]      argc Count of the command line arguments
** @param [in]      argv The argument vector
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_args_set(gf_args* args, int* argc, char*** argv);

/*!
** @brief Inherit the argument state from the other.
**
** @param [out] dst The argument object, which is inherit the argument state
** @param [in]  src The argument object, which is inherited by <code>dst</code>
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_args_inherit(gf_args* dst, gf_args* src);

/*!
** @brief Return the count of the remaining arguments.
**
** @param [in] args The argument object
*/

extern int gf_args_remain(const gf_args* args);

/*!
** @brief Read the command option string and move forward the pointer which
** points to the argument string.
**
** @param [in, out] args The argument object
** @param [out]     str  The pointer to the command argument
*/

extern gf_status gf_args_consume(gf_args* args, char** str);

/*!
**
**
*/

extern gf_bool gf_args_is_specified(const gf_args* args, int key);

/*!
** @brief Get the argument entry object by the short option as a key.
**
** @param [out] entry  The argument entry object
** @param [in]  option The short option
** @param [in]  args   The argument object
*/

extern gf_status gf_args_get_entry_by_short_option(
  gf_args_entry** entry, char option, const gf_args* args);

/*!
** @brief Get the argument entry object by the long option as a key.
**
** @param [out] entry  The argument entry object
** @param [in]  option The long option
** @param [in]  args   The argument object
*/

extern gf_status gf_args_get_entry_by_long_option(
  gf_args_entry** entry, const char* option, const gf_args* args);

/*!
** @brief Get the option argument vector.
**
** @param [in]  args      The argument object
** @param [in]  key       The args index
** @param [out] opt_args  The option argument vector
** @param [out] opt_count The count of elements of the option arguments
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_args_get_option_args(
  gf_args* args, int key, char*** opt_args, gf_size_t* opt_count);

extern void gf_args_print_help(const gf_args* args);

/* -------------------------------------------------------------------------- */

/*!
** @brief Create a command argument entry
**
** This object is mainly used internally.
**
** @param [in, out] entry       The argument entry object
** @param [in]      key         The argument entry key
** @param [in]      opt_short   A short option
** @param [in]      opt_long    A long option
** @param [in]      arg_count   Count of the option arguments
** @param [in]      description The description of the argument
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_status gf_args_entry_new(
  gf_args_entry** entry, int key, char opt_short, const char* opt_long,
  uint8_t arg_count, const char* description);

/*!
** @brief Destroy a specified argument entry.
**
** @param [in] entry The argument entry object to destroy
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise.
*/

extern void gf_args_entry_free(gf_args_entry* entry);

/*!
** @brief Test if the argument entry is specifed.
**
** @param [in] entry The argument entry object.
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise.
*/

extern gf_bool gf_args_entry_is_specified(const gf_args_entry* entry);

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_ARGS_H */
