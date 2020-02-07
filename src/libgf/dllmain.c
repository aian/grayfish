/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/dllmain.c
** @brief DLL main.
*/
#include <libgf/config.h>

#include <stdio.h>
#include <stdlib.h>

#include <windows.h>

#include <libgf/gf_global.h>

BOOL WINAPI
DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpReserved) {
  BOOL ret = TRUE;

  (void)hInst;
  (void)lpReserved;

  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    ret = gf_global_init() != GF_SUCCESS ? FALSE : TRUE;
    break;
  case DLL_PROCESS_DETACH:
    ret = gf_global_clean() != GF_SUCCESS ? FALSE : TRUE;
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  default:
    ret = FALSE;
    break;
  }

  return ret;
}
