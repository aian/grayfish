/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file test/util.c
** @brief Utilities for testing
*/
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#ifndef INITGUID
#define INITGUID
#endif

#include <objbase.h>
#include <windows.h>

#include "util.h"

#ifdef BUFSIZE
#undef BUFSIZE
#endif
#define BUFSIZE 256


struct gft_test_ctxt {
  char* root_path;
  char* work_path;
  char* orig_path;
};

static int
test_ctxt_get_current_path(char** path) {
  DWORD len = 0;
  char* tmp = NULL;

  len = GetCurrentDirectory(0, NULL);
  if (len <= 0) {
    return 1;
  }
  tmp = malloc(len * sizeof(char));
  if (!tmp) {
    return 1;
  }
  (void)GetCurrentDirectory(len, tmp);

  *path = tmp;
  
  return 0;
}

/*
** @brief Check if the specified path exists and is writable
*/

bool
test_ctxt_is_path_writable_directory(const char* path) {
  DWORD flags = 0;
  bool ret = false;
  
  if (!path || !path[0]) {
    return false;
  }
  /* Check if the path is writable directory */
  flags = GetFileAttributes(path);
  if (flags == INVALID_FILE_ATTRIBUTES) {
    ret = false;
  } else if (flags & FILE_ATTRIBUTE_READONLY) {
    ret = false;
  } else if (!(flags & FILE_ATTRIBUTE_DIRECTORY)) {
    ret = false;
  } else {
    ret = true;
  }

  return ret;
}

static int
test_ctxt_get_root_path(char** path) {
  int rc = 0;
  char* tmp = NULL;
  DWORD len = 0;

  if (!path) {
    return 1;
  }
  /* Get temporary path of the system */
  len = GetTempPath(0, NULL);
  if (len > 0) {
    tmp = malloc(len * sizeof(char));
    if (!tmp) {
      return 1;
    }

    (void)GetTempPath(len, tmp);
  
    if (test_ctxt_is_path_writable_directory(tmp)) {
        free(tmp);
        tmp = NULL;
    }
  }
  /* Check if the tmp path is alive */
  if (!tmp) {
    rc = test_ctxt_get_current_path(&tmp);
    if (rc != 0) {
      return 1;
    }
    assert(tmp);
    assert(tmp[0]);
    if (!test_ctxt_is_path_writable_directory(tmp)) {
      free(tmp);
      return 1;
    }
  }

  *path = tmp;

  return 0;
}

static int
test_ctxt_get_work_path(char** path, const char* root) {
  char* tmp = NULL;
  wchar_t w_str[BUFSIZE] = { 0 }; 
  char a_str[BUFSIZE] = { 0 }; 
  size_t len = 0;

  GUID guid = { 0 };

  static const char prefix[] = "gft-";
  static const char delimiter[] = "\\";

  if (!path || !root) {
    return 1;
  }
  
  /* Make GUID string*/
  if (CoCreateGuid(&guid) != S_OK) {
    return 1;
  }
  (void)StringFromGUID2(&guid, w_str, BUFSIZE);
  (void)WideCharToMultiByte(CP_UTF8, 0, w_str, -1, a_str, BUFSIZE, NULL, NULL);

  /* Allocate work path buffer */
  len = strlen(root) + strlen(delimiter) + strlen(prefix) + strlen(a_str);
  assert(len > 0);
  tmp = malloc((len + 1) * sizeof(char));
  if (!tmp) {
    return 1;
  }

  /* Build the work path */
  _snprintf_s(tmp, len, _TRUNCATE, "%s%s%s%s", root, delimiter, prefix, a_str);

  *path = tmp;
  
  return 0;
}

static int
test_ctxt_prepare(gft_test_ctxt* ctxt) {
  int rc = 0;

  if (!ctxt) {
    return 1;
  }
  /* Prepare */
  rc = test_ctxt_get_root_path(&ctxt->root_path);
  if (rc != 0) {
    return rc;
  }
  rc = test_ctxt_get_work_path(&ctxt->work_path, ctxt->root_path);
  if (rc != 0) {
    return rc;
  }
  rc = test_ctxt_get_current_path(&ctxt->orig_path);
  if (rc != 0) {
    return rc;
  }
  
  return 0;
}

static int
test_ctxt_init(gft_test_ctxt* ctxt) {
  if (!ctxt) {
    return 1;
  }
  /* Initialize */
  ctxt->root_path = NULL;
  ctxt->work_path = NULL;
  ctxt->orig_path = NULL;
  
  return 0;
}

int
gft_test_ctxt_new(gft_test_ctxt** ctxt) {
  int rc = 0;
  gft_test_ctxt* tmp = NULL;
  
  if (!ctxt) {
    return 1;
  }
  tmp = malloc(sizeof(*tmp));
  if (!tmp) {
    return 1;
  }
  rc = test_ctxt_init(tmp);
  if (rc != 0) {
    free(tmp);
    return rc;
  }
  rc = test_ctxt_prepare(tmp);
  if (rc != 0) {
    gft_test_ctxt_free(tmp);
    return rc;
  }
  *ctxt = tmp;
  
  return 0;
}

void
gft_test_ctxt_free(gft_test_ctxt* ctxt) {
  if (ctxt) {
    if (ctxt->root_path) {
      free(ctxt->root_path);
      ctxt->root_path = NULL;
    }
    if (ctxt->work_path) {
      // TODO: Change current path back to the original path
      // TODO: Cleanup the work path
      free(ctxt->work_path);
      ctxt->work_path = NULL;
    }
    if (ctxt->orig_path) {
      free(ctxt->orig_path);
      ctxt->orig_path = NULL;
    }
    free(ctxt);
  }
}
