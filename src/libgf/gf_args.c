/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_args.c
** @brief Operations for command arguments.
*/
#include <string.h>

#include <libgf/gf_memory.h>
#include <libgf/gf_string.h>
#include <libgf/gf_log.h>
#include <libgf/gf_args.h>

#include "gf_local.h"

/*!
** @brief Test if the specified option object is null
**
** 
**
*/
static gf_bool
option_is_null(const gf_option* option) {
  static const gf_option nul = GF_OPTION_NULL;

  if (!option) {
    return GF_FALSE;
  }
  if (option->key != nul.key) {
    return GF_FALSE;
  }
  if (option->opt_short != nul.opt_short) {
    return GF_FALSE;
  }
  if (option->opt_long != nul.opt_long) {
    return GF_FALSE;
  }
  if (option->opt_count != nul.opt_count) {
    return GF_FALSE;
  }
  if (option->usage != nul.usage) {
    return GF_FALSE;
  }
  if (option->description != nul.description) {
    return GF_FALSE;
  }

  return GF_TRUE;
};

/* -------------------------------------------------------------------------- */

struct gf_args_entry {
  int     key;
  gf_bool is_specified; ///< A boolean value if this argument is specified
  int8_t  arg_count;    ///< The number of the option arguments.(0 upto 255)
  uint8_t arg_used;     ///< The count of the specified option argument vector
  uint8_t arg_size;     ///< The size of the option argument vector
  char    opt_short;    ///< Short option represented as "-?"
  char**  opt_args;     ///< Option arguments
  char*   opt_long;     ///< Long option represented as "--????"
  char*   description;  ///< A description of this option
  
};

/*!
** @brief Initialize the command argument entry
**
** @param [in, out] entry The argument entry object to initialize
**
** @return Retuens GF_SUCCESS on success, GF_E_* otherwise
*/

static gf_status
args_entry_init(gf_args_entry* entry) {
  gf_validate(entry);

  entry->key = 0;
  entry->is_specified = GF_FALSE;
  entry->arg_count = 0;
  entry->arg_used = 0;
  entry->arg_size = 0;
  entry->opt_short = '\0';
  entry->opt_args = NULL;
  entry->opt_long = NULL;
  entry->description = NULL;
  
  return GF_SUCCESS;
}

gf_status
gf_args_entry_new(
  gf_args_entry** entry, int key, char opt_short, const char* opt_long,
  uint8_t arg_count, const char* description) {

  gf_status rc = 0;
  gf_args_entry* tmp = NULL;
  
  gf_validate(entry);
  gf_validate(opt_short || !gf_strnull(opt_long));
  
  _(gf_malloc((gf_ptr* )&tmp, sizeof(*tmp)));

  rc = args_entry_init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    return rc;
  }
  {
    tmp->key = key;
    tmp->opt_short = opt_short;
    
    rc = gf_strdup(&tmp->opt_long, opt_long);
    if (rc != GF_SUCCESS) {
      gf_args_entry_free(tmp);
      return rc;
    }

    tmp->arg_count = arg_count;

    rc = gf_strdup(&tmp->description, description);
    if (rc != GF_SUCCESS) {
      gf_args_entry_free(tmp);
      return rc;
    }
  }
  *entry = tmp;
    
  return GF_SUCCESS;
}

void
gf_args_entry_free(gf_args_entry* entry) {
  if (entry) {
    entry->arg_count = 0;
    
    if (entry->opt_args) {
      for (uint8_t i = 0; i < entry->arg_count; i++) {
        if (entry->opt_args[i]) {
          gf_free(entry->opt_args[i]);
          entry->opt_args[i] = NULL;
        }
      }
    }

    entry->opt_short = '\0';
    
    if (entry->opt_long) {
      gf_free(entry->opt_long);
      entry->opt_long = NULL;
    }

    if (entry->description) {
      gf_free(entry->description);
      entry->description = NULL;
    }

    gf_free(entry);
  }
}

gf_bool
gf_args_entry_is_specified(const gf_args_entry* entry) {
  return (entry && entry->is_specified) ? GF_TRUE : GF_FALSE;
}

/*!
** @brief Add an option argument.
**
** @param [in, out] entry 
** @param [in]      index
** @param [in]      begin 
** @param [in]      end 
*/

static gf_status
args_entry_add_option_args(
  gf_args_entry* entry, uint8_t index, const char* begin, const char* end) {
  gf_status rc = 0;
  gf_size_t len = 0;
  char* str = NULL;

  gf_validate(entry);
  gf_validate(begin);
  gf_validate(end);

  if (begin >= end) {
    gf_raise(GF_E_STATE, "Invalid option argument range.");
  }
  if ((index + 1) > entry->arg_count) {
    gf_raise(GF_E_COMMAND, "Too many option arguments.");
  }
  /* Prepare the string buffer */
  len = ((uintptr_t)(end - begin)) / sizeof(char);
  _(gf_malloc((gf_ptr*)&str, len + 1));
  /* Copy string */
  for (gf_size_t i = 0; i < len; i++) {
    str[i] = begin[i];
  }
  str[len] = '\0';
  /* Assign */
  if (entry->arg_size < (index + 1)) {
    gf_size_t bufsize = sizeof(char*) * entry->arg_count;
    rc = gf_realloc((gf_ptr*)&entry->opt_args, bufsize);
    if (rc != GF_SUCCESS) {
      gf_free(str);
      return rc;
    }
    entry->arg_size = entry->arg_count;
  }
  entry->opt_args[index] = str;
  entry->arg_used += 1;
  
  return GF_SUCCESS;
}

/* -------------------------------------------------------------------------- */

#ifndef GF_ARGS_ENTRY_COUNT
#define GF_ARGS_ENTRY_COUNT 8
#endif  /* GF_ARGS_ENTRY_COUNT */

struct gf_args {
  gf_args_entry** entries;
  gf_size_t used;
  gf_size_t size;
  int* argc;
  char*** argv;
};

static gf_status
args_init(gf_args* args) {
  gf_args_entry** tmp = NULL;
  
  static const gf_size_t size = sizeof(*tmp) * GF_ARGS_ENTRY_COUNT;

  gf_validate(args);
  _(gf_malloc((gf_ptr* )&tmp, size));

  args->entries = tmp;
  args->used = 0;
  args->size = GF_ARGS_ENTRY_COUNT;
  args->argc = NULL;
  args->argv = NULL;
  
  return GF_SUCCESS;
}

gf_status
gf_args_new(gf_args** args) {
  gf_status rc = 0;
  gf_args* tmp = NULL;

  gf_validate(args);

  _(gf_malloc((gf_ptr* )&tmp, sizeof(*tmp)));
  rc = args_init(tmp);
  if (rc != GF_SUCCESS) {
    gf_free(tmp);
    return rc;
  }
  *args = tmp;

  return GF_SUCCESS;
}

void
gf_args_free(gf_args* args) {
  if (args) {
    if (args->entries) {
      for (gf_size_t i = 0; i < args->used; i++) {
        if (args->entries[i]) {
          gf_args_entry_free(args->entries[i]);
          args->entries[i] = NULL;
        }
      }
      gf_free(args->entries);
      args->entries = NULL;
    }
    args->used = 0;
    args->size = 0;

    gf_free(args);
  }
}

static gf_status
args_add_entry(
  gf_args* args, int key, char opt_short, const char* opt_long,
  uint8_t arg_count, const char* description) {

  gf_validate(args);
  
  /* Extend the argument entries array */
  if (args->used >= args->size) {
    gf_size_t size = sizeof(*args->entries) * args->size * 2;
    _(gf_realloc((gf_ptr* )&args->entries, size));
    args->size = size;
  }
  {
    gf_args_entry* entry = NULL;

    _(gf_args_entry_new(
        &entry, key, opt_short, opt_long, arg_count, description));

    args->entries[args->used] = entry;
    args->used += 1;
  }
  
  return GF_SUCCESS;
}

gf_status
gf_args_add_option(gf_args* args, const gf_option* option) {
  /* alias */
  const gf_option* o = option;
  
  gf_validate(args);
  gf_validate(option);

  _(args_add_entry(
      args, o->key, o->opt_short, o->opt_long, o->opt_count, o->description));

  return GF_SUCCESS;
}

gf_status
gf_args_add_option_table(gf_args* args, const gf_option* options) {
  gf_validate(args);
  gf_validate(options);

  for (const gf_option* ptr = options; !option_is_null(ptr); ptr++) {
    _(gf_args_add_option(args, ptr));
  }
  
  return GF_SUCCESS;
}

/*!
** @brief Parse the long option arguments.
**
** @param [in, out] entry The argument entry object
** @param [in]      str   The string of the command argument
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise.
*/

static gf_status
args_parse_long_option_args(gf_args_entry* entry, const char* str) {
  gf_size_t arg_index = 0;
  const char* begin = NULL;

  enum { IN_, OUT_, } state = IN_;

  if (*str == '\0') {
    return GF_SUCCESS;
  }
  
  begin = str;
  for (const char* ptr = str + 1; ; ptr++) {
    switch (*ptr) {
    case ',':
    case '\0':
      if (state == IN_) {
        _(args_entry_add_option_args(entry, arg_index, begin, ptr));
      }
      state = OUT_;
      if (*ptr == '\0') {
        return GF_SUCCESS;
      }
      break;
    default:
      if (state == OUT_) {
        begin = ptr;
      }
      state = IN_;
      break;
    }
  }
  
  return GF_SUCCESS;
}

/*!
** @brief Parse the short option arguments.
**
** @param [in, out] args  The argument object
** @param [in]      entry The argument entry object
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise.
*/

static gf_status
args_parse_short_option_args(gf_args* args, gf_args_entry* entry) {  
  for (uint8_t i = 0; i < entry->arg_count; i++) {
    const char* begin = NULL;
    const char* end = NULL;

    if (*args->argc <= 0) {
      gf_raise(GF_E_COMMAND, "Too short arguments.");
    }

    begin = *args->argv[0];
    end = begin + strlen(begin);

    _(args_entry_add_option_args(entry, i, begin, end));

    *args->argc -= 1;
    *args->argv += 1;
  }

  return GF_SUCCESS;
}

/*!
** @brief Parse the long option.
**
** @param [in, out] args  The argument object
** @param [in]      str   The string of the command argument
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise.
*/

static gf_status
args_parse_long_option(gf_args* args, const char* str) {
  gf_validate(args);
  gf_validate(args->entries);
  gf_validate(!gf_strnull(str));
  
  for (uint8_t i = 0; i < args->used; i++) {
    if (args->entries[i] && !gf_strnull(args->entries[i]->opt_long)) {
      const gf_size_t len = strlen(args->entries[i]->opt_long);
      if (strncmp(args->entries[i]->opt_long, str, len) != 0) {
        continue;
      }
      switch (str[len]) {
      case '=':
        _(args_parse_long_option_args(args->entries[i], &str[len + 1]));
        /* fall-through */
      case '\0':
        args->entries[i]->is_specified = GF_TRUE;
        break;
      default:
        continue;
        break;
      }
    }
  }
  /* One command argument has been processed. */
  *(args->argc) -= 1;
  *(args->argv) += 1;

  return GF_SUCCESS;
}

/*!
** @brief Parse the short option.
**
** @param [in, out] args  The argument object
** @param [in]      chr   The short option character
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise.
*/

static gf_status
args_parse_short_option(gf_args* args, char chr) {
  gf_validate(args);
  gf_validate(args->entries);

  for (uint8_t i = 0; i < args->used; i++) {
    if (args->entries[i]) {
      if (args->entries[i]->opt_short == chr) {
        gf_args_entry* entry = args->entries[i];

        entry->is_specified = GF_TRUE;

        _(gf_args_consume(args, NULL));

        if (entry->arg_count > 0) {
          _(args_parse_short_option_args(args, entry));
        }
        return GF_SUCCESS;
      }
    }
  }
  
  gf_raise(GF_E_OPTION, "Unknown command option '-%c'.", chr);
}

gf_status
gf_args_parse(gf_args* args) {
  gf_validate(args);

  if (!args->argc || !args->argv) {
    gf_error("Command argument is not set.");
    gf_raise(GF_E_STATE, "Command argument is not set.");
  }
  while (*(args->argc) > 0) {
    const char* str = *(args->argv)[0];
    const gf_size_t len = strlen(str);

    if (gf_strnull(str) || len < 2) {
      /* Finish parsing */
      break;
    }
    if (str[0] == '-') {
      if (str[1] == '-') {
        /* Long option */
        _(args_parse_long_option(args, &str[2]));
      } else if (str[2] == '\0') {
        /* Short option */
        _(args_parse_short_option(args, str[1]));
      } else {
        gf_raise(GF_E_COMMAND, "Invalid command argument.");
      }
    } else {
      /* Finish parsing */
      break;
    }
  }
  
  return GF_SUCCESS;
}

gf_status
gf_args_set(gf_args* args, int* argc, char*** argv) {
  gf_validate(args);
  gf_validate(argc);
  gf_validate(argv);

  args->argc = argc;
  args->argv = argv;

  return GF_SUCCESS;
}

gf_status
gf_args_inherit(gf_args* dst, gf_args* src) {
  gf_validate(dst);
  gf_validate(src);

  dst->argc = src->argc;
  dst->argv = src->argv;

  return GF_SUCCESS;
}


int
gf_args_remain(const gf_args* args) {
  if (!args || !args->argc) {
    return 0;
  }
  return* args->argc;
}

gf_status
gf_args_consume(gf_args* args, char** str) {
  char* tmp = NULL;
  const int argc = gf_args_remain(args);

  gf_validate(args);

  if (argc <= 0) {
    gf_raise(GF_E_STATE, "No command argument remains");
  }
  if (str) {
    _(gf_strdup(&tmp,* args->argv[0]));
    *str = tmp;
  }

  *args->argc -= 1;
  *args->argv += 1;
  
  return GF_SUCCESS;
}

gf_bool
gf_args_is_specified(const gf_args* args, int key) {
  if (!args || (args->used > 0 && !args->entries)) {
    return GF_FALSE;
  }
  for (gf_size_t i = 0; i < args->used; i++) {
    gf_args_entry* e = args->entries[i];
    if (e->key == key) {
      return gf_args_entry_is_specified(e);
    }
  }
  /* No argument entry has been found. */
  return GF_FALSE;
}

gf_status
gf_args_get_entry(gf_args_entry** entry, int key, const gf_args* args) {
  gf_validate(entry);
  gf_validate(args);

  for (gf_size_t i = 0; i < args->used; i++) {
    gf_args_entry* e = args->entries[i];
    if (e->key == key) {
      *entry = e;
      return GF_SUCCESS;
    }
  }
  /* No argument entry has been found. */
  gf_raise(GF_E_INTERNAL,
           "The command option specified by %d is not found", key);
}

gf_status
gf_args_get_entry_by_short_option(
  gf_args_entry** entry, char option, const gf_args* args) {
  gf_validate(entry);
  gf_validate(args);

  for (gf_size_t i = 0; i < args->used; i++) {
    gf_args_entry* e = args->entries[i];
    if (e->opt_short == option) {
      *entry = e;
      return GF_SUCCESS;
    }
  }
  /* No argument entry has been found. */
  gf_raise(GF_E_INTERNAL,
           "The command option specified by '-%c' is not found", option);
}

gf_status
gf_args_get_entry_by_long_option(
  gf_args_entry** entry, const char* option, const gf_args* args) {
  gf_validate(entry);
  gf_validate(!gf_strnull(option));
  gf_validate(args);

  for (gf_size_t i = 0; i < args->used; i++) {
    gf_args_entry* e = args->entries[i];
    if (!strcmp(e->opt_long, option)) {
      *entry = e;
      return GF_SUCCESS;
    }
  }
  /* No argument entry has been found. */
  gf_raise(GF_E_INTERNAL,
           "The command option specified by '--%s' is not found", option);
}

gf_status
gf_args_get_option_args(
  gf_args* args, int key, char*** opt_args, gf_size_t* opt_count) {
  gf_args_entry* entry = NULL;
  
  gf_validate(args);
  gf_validate(opt_args);
  gf_validate(opt_count);

  _(gf_args_get_entry(&entry, key, args));

  *opt_args = entry->opt_args;
  *opt_count = entry->arg_count;

  
  return GF_SUCCESS;
}

void
gf_args_print_help(const gf_args* args) {
  assert(args);
  if (args) {
    gf_char name[1024] = { 0 };
    
    
    for (size_t i = 0; i < args->used; i++) {
      const gf_args_entry* e = args->entries[i];

      sprintf_s(
        name, 1024, "%s%c%s%s%s",
        e->opt_short ? "-" : "",
        e->opt_short ? e->opt_short : '\0',
        (e->opt_short && !gf_strnull(e->opt_long)) ? ", " : "",
        !gf_strnull(e->opt_long) ? "--" : "",
        !gf_strnull(e->opt_long) ? e->opt_long : ""
        );

      gf_msg("    %-24s%s", name, e->description);
    }
  }
}
