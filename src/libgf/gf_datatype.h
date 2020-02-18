/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_datatype.h
** @brief Primitive data types.
*/
#ifndef LIBGF_GF_DATATYPE_H
#define LIBGF_GF_DATATYPE_H

#pragma once

#include <libgf/config.h>

#include <wchar.h>
#include <stddef.h>

typedef _Bool   gf_bool;
typedef size_t  gf_size_t;
typedef wchar_t gf_char;    /* For now, this code is for Winddows OS */
typedef void*   gf_ptr;

extern const gf_bool GF_TRUE;
extern const gf_bool GF_FALSE;

enum gf_status {
  GF_SUCCESS   = 0,  ///< Success
  GF_E_COMMAND,
  GF_E_OPTION,
  GF_E_PARAM,
  GF_E_CONFIG,
  GF_E_ALLOC,
  GF_E_OPEN,
  GF_E_READ,
  GF_E_WRITE,
  GF_E_PATH,         ///< Invalid path condition
  GF_E_API,
  GF_E_STATE,        ///< Invalid process state
  GF_E_EXEC,
  GF_E_INTERNAL,     ///< Internal error
};

typedef enum gf_status gf_status;

#endif  /* LIBGF_GF_DATATYPE_H */
