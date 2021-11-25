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
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef _Bool       gf_bool;
typedef size_t      gf_size_t;
typedef char        gf_char;
typedef int         gf_int;
typedef int8_t      gf_8s;
typedef uint8_t     gf_8u;
typedef int16_t     gf_16s;
typedef uint16_t    gf_16u;
typedef int32_t     gf_32s;
typedef uint32_t    gf_32u;
typedef int64_t     gf_64s;
typedef uint64_t    gf_64u;
typedef void*       gf_ptr;
typedef const void* gf_const_ptr;

extern const gf_bool GF_TRUE;
extern const gf_bool GF_FALSE;

/*!
** @brief 64-bit any datatype
*/

union gf_any {
  gf_size_t size;
  gf_8s     s8;
  gf_8u     u8;
  gf_16s    s16;
  gf_16u    u16;
  gf_32s    s32;
  gf_32u    u32;
  gf_64s    s64;
  gf_64u    u64;
  gf_ptr    ptr;
  gf_8s     ary_s8[8];
  gf_8u     ary_u8[8];
  gf_16s    ary_s16[4];
  gf_16u    ary_u16[4];
  gf_32s    ary_s32[2];
  gf_32u    ary_u32[2];
  gf_64u    data;
};

typedef union gf_any gf_any;

#ifdef __cplusplus
}
#endif

#endif  /* LIBGF_GF_DATATYPE_H */
