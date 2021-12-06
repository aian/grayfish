/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file test/test-array.c
** @brief Testing module for gf_array.
*/
#include <CUnit/CUnit.h>

#include <libgf/gf_xslt.h>

#include "local.h"

/* -------------------------------------------------------------------------- */




/* -------------------------------------------------------------------------- */

/*!
** @brief The interface function for the test of gf_array.
**
** Registers the tests of gf_array module.
*/

void
gft_xslt_add_tests(void) {
  CU_pSuite s = CU_add_suite("Tests for gf_xslt", NULL, NULL);
  (void)s;
}
