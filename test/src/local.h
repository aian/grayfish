/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file test/local.h
** @brief Common definitions.
*/
#ifndef TEST_LIBGF_LOCAL_H_
#define TEST_LIBGF_LOCAL_H_

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"

extern void gft_string_add_tests(void);
extern void gft_path_add_tests(void);
extern void gft_shell_add_tests(void);
extern void gft_array_add_tests(void);
extern void gft_file_info_add_tests(void);
extern void gft_site_add_tests(void);

#ifdef __cplusplus
}
#endif

#endif  /* TEST_LIBGF_LOCAL_H_ */
