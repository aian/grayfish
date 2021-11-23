/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_datetime.c
** @brief Datetime management
*/
#include <ctype.h>
#include <time.h>

#include <libgf/gf_memory.h>
#include <libgf/gf_datetime.h>

#include "gf_local.h"

static gf_int
date_parse_digit(const gf_char** ptr, gf_int digit) {
  gf_int n = 0;

  if (!ptr || digit <= 0) {
    return -1;
  }
  for (gf_int i = 0; i < digit; i++) {
    if (!(*ptr)[i] || !isdigit((*ptr)[i])) {
      return -1;
    }
    n = (n * 10) + ((*ptr)[i] - '0');
  }
  (*ptr) += digit;
  
  return n;
};

static gf_int
date_parse_char(const gf_char** ptr, gf_char chr) {
  gf_int ret = 0;
  
  ret = ptr && (*ptr)[0] == chr ? 1 : -1;
  (*ptr)++;
  
  return ret;
};

#define DATE_VALIDATE_TOKEN(n, str)                             \
  do {                                                          \
    if ((n) < 0) {                                              \
      gf_raise(GF_E_DATA, "Invalid date string. (%s)", (str));  \
    }                                                           \
  } while (0)

gf_status
gf_date_parse(const gf_char* str, gf_datetime* datetime) {
  const gf_char* cur = NULL;
  gf_int n = 0;
  gf_64u tmp = 0;
  struct tm tm = { 0 };
  
  gf_validate(str);
  gf_validate(datetime);

  _Static_assert(sizeof(time_t) == 8);
  
  cur = str;

  /* YYYY */
  n = date_parse_digit(&cur, 4);
  DATE_VALIDATE_TOKEN(n, str);
  tm.tm_year = n - 1900;
  /* - */
  n = date_parse_char(&cur, '-');
  DATE_VALIDATE_TOKEN(n, str);
  /* MM */
  n = date_parse_digit(&cur, 2);
  DATE_VALIDATE_TOKEN(n, str);
  tm.tm_mon = n - 1;
  /* - */
  n = date_parse_char(&cur, '-');
  DATE_VALIDATE_TOKEN(n, str);
  /* DD */
  n = date_parse_digit(&cur, 2);
  DATE_VALIDATE_TOKEN(n, str);
  tm.tm_mday = n;
  /* delimiter */
  n = date_parse_char(&cur, ' ');
  DATE_VALIDATE_TOKEN(n, str);
  /* HH */
  n = date_parse_digit(&cur, 2);
  DATE_VALIDATE_TOKEN(n, str);
  tm.tm_hour = n;
  /* : */
  n = date_parse_char(&cur, ':');
  DATE_VALIDATE_TOKEN(n, str);
  /* mm */
  n = date_parse_digit(&cur, 2);
  DATE_VALIDATE_TOKEN(n, str);
  tm.tm_min = n;
  /* : */
  n = date_parse_char(&cur, ':');
  DATE_VALIDATE_TOKEN(n, str);
  /* SS */
  n = date_parse_digit(&cur, 2);
  DATE_VALIDATE_TOKEN(n, str);
  tm.tm_sec = n;
  /* NUL */
  if (*cur != '\0') {
    DATE_VALIDATE_TOKEN(-1, str);
  }
  /*  */
  tmp = (gf_64u)(mktime(&tm));
  if ((gf_64s)tmp == -1) {
    DATE_VALIDATE_TOKEN(-1, str);
  }
  
  *datetime = tmp;
  
  return GF_SUCCESS;
}

gf_status
gf_date_make_string(gf_string* str, gf_datetime datetime) {
  errno_t err = 0;
  struct tm tm = { 0 };
  gf_char buf[256] = { 0 };

  err = localtime_s(&tm, (time_t*)&datetime);
  if (err != 0) {
    gf_raise(GF_E_DATA, "Failed to make a datetime string.");
  }
  strftime(buf, 256, "%Y-%m-%d %H:%M:%S", &tm);

  _(gf_string_set(str, buf));

  return GF_SUCCESS;
}
