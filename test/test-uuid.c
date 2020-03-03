/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file test/test-uuid.c
** @brief Testing module for gf_uuid.
*/
#include <CUnit/CUnit.h>

#include <libgf/gf_uuid.h>

#include "local.h"

/* -------------------------------------------------------------------------- */

static void
parse_normal(void) {
  gf_status rc = 0;
  const char str[] = "{01234567-89AB-CDEF-0123-456789ABCDEF}";
  gf_uuid uuid = { 0 };

  rc = gf_uuid_parse(&uuid, str);
  CU_ASSERT_EQUAL(rc, GF_SUCCESS);
  CU_ASSERT_EQUAL(uuid.data_1, 0x01234567);
  CU_ASSERT_EQUAL(uuid.data_2, 0x89AB);
  CU_ASSERT_EQUAL(uuid.data_3, 0xCDEF);
  CU_ASSERT_EQUAL(uuid.data_4[0], 0x01);
  CU_ASSERT_EQUAL(uuid.data_4[1], 0x23);
  CU_ASSERT_EQUAL(uuid.data_4[2], 0x45);
  CU_ASSERT_EQUAL(uuid.data_4[3], 0x67);
  CU_ASSERT_EQUAL(uuid.data_4[4], 0x89);
  CU_ASSERT_EQUAL(uuid.data_4[5], 0xAB);
  CU_ASSERT_EQUAL(uuid.data_4[6], 0xCD);
  CU_ASSERT_EQUAL(uuid.data_4[7], 0xEF);
}

/* -------------------------------------------------------------------------- */

/*!
** @brief The interface function for the test of gf_uuid.
**
** Registers the tests of gf_uuid module.
*/

void
gft_uuid_add_tests(void) {
  CU_pSuite s = CU_add_suite("Tests for gf_uuid", NULL, NULL);

  /* parse */
  CU_add_test(s, "Parse in noraml case",   parse_normal);
}
