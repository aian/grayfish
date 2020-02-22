/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file test/util.h
** @brief Utilities for testing
*/
#ifndef TEST_LIBGF_UTIL_H_
#define TEST_LIBGF_UTIL_H_

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*! 
** @brief Testing context handle
*/

typedef struct gft_test_ctxt gft_test_ctxt;

/*!
** @brief Create a test environment and returns the handle
**
** @param [out] ctxt The created testing context
*/

extern int gft_test_ctxt_new(gft_test_ctxt** ctxt);

/*!
** @brief Destroy a test handle 
**
** @param [in] ctxt The test context to be destroyed
*/

extern void gft_test_ctxt_free(gft_test_ctxt* ctxt);


#ifdef __cplusplus
}
#endif

#endif  /* TEST_LIBGF_UTIL_H_ */
