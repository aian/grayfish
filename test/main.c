/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file test/main.c
** @brief The entry point for unit testing.
*/
#include <stdlib.h>

#include <CUnit/Cunit.h>
#include <CUnit/Console.h>

#include "local.h"

void
gft_init(void) {
  CU_initialize_registry();
}

void
gft_prepare(void) {
  gft_path_add_tests();  // gf_path
}

void
gft_run(void) {
  CU_console_run_tests();
}

void
gft_cleanup(void) {
  CU_cleanup_registry();
}


/*!
** @brief The entry point
**
** @param argc [in] Count of the command argument.
** @param argv [in] The vector of the command argument.
*/

int
main(void) {
  int ret = EXIT_SUCCESS;

  gft_init();

  gft_prepare();

  gft_run();
  
  gft_cleanup();
   
  return ret;
}
