/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_cmd_base.c
** @brief Abstract command interface and factory.
*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <libgf/gf_string.h>
#include <libgf/gf_memory.h>
#include <libgf/gf_log.h>

#include <libgf/gf_cmd_base.h>

#include "gf_local.h"

#define COMMAND_NAME_DEFAULT "<NO NAME>"
#define COMMAND_DESCRIPTION_DEFAULT ""

gf_status
gf_command_init(gf_command* cmd) {
  gf_validate(cmd);

  cmd->name = NULL;
  cmd->description = NULL;
  cmd->args = NULL;
  cmd->free = NULL;
  cmd->execute = NULL;

  return GF_SUCCESS;
}

gf_status
gf_command_prepare(gf_command* cmd) {
  gf_status rc = 0;
  gf_validate(cmd);

  /* Name */
  rc = gf_strassign(&cmd->name, COMMAND_NAME_DEFAULT);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  /* Description */
  rc = gf_strassign(&cmd->description, COMMAND_DESCRIPTION_DEFAULT);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  /* Argument */
  if (cmd->args) {
    gf_args_free(cmd->args);
    cmd->args = NULL;
  }
  rc = gf_args_new(&cmd->args);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  /* Virtual functions defined by the drived objects. */
  cmd->free = NULL;
  cmd->execute = NULL;

  return GF_SUCCESS;
}

void
gf_command_clear(gf_command* cmd) {
  if (cmd) {
    /* Base structure member */
    if (cmd->name) {
      gf_free(cmd->name);
    }
    if (cmd->description) {
      gf_free(cmd->description);
    }
    if (cmd->args) {
      gf_args_free(cmd->args);
    }
    (void)gf_command_init(cmd);
  }
}

gf_status
gf_command_set_info(gf_command* cmd, const gf_command_info* info) {
  gf_validate(cmd);
  gf_validate(info);

  _(gf_command_prepare(cmd));
  _(gf_command_set_name(cmd, info->base.name));
  _(gf_command_set_description(cmd, info->base.description));
  _(gf_command_add_options(cmd, info->options));

  cmd->create = info->base.create;
  cmd->free = info->base.free;
  cmd->execute = info->base.execute;
  
  return GF_SUCCESS;
}


gf_status
gf_command_add_options(gf_command* cmd, const gf_option* options) {
  gf_validate(cmd);
  gf_validate(options);

  _(gf_args_add_option_table(cmd->args, options));
  
  return GF_SUCCESS;
}

gf_status
gf_command_set_args(gf_command* cmd, int* argc,  char*** argv) {
  gf_validate(cmd);
  gf_validate(argc);
  gf_validate(argv);

  _(gf_args_set(cmd->args, argc, argv));

  return GF_SUCCESS;
}

gf_status
gf_command_inherit_args(gf_command* dst, const gf_command* src) {
  gf_validate(dst);
  gf_validate(src);
  gf_validate(dst->args);
  gf_validate(src->args);

  _(gf_args_inherit(dst->args, src->args));

  return GF_SUCCESS;
}

gf_status
gf_command_consume_args(gf_command* cmd, char** str) {
  gf_validate(cmd);

  if (!cmd->args) {
    gf_debug("cmd->args is NULL.");
    gf_raise(GF_E_STATE, "Command argument is not set.");
  }
  _(gf_args_consume(cmd->args, str));
  
  return GF_SUCCESS;
}

gf_status
gf_command_set_name(gf_command* cmd, const char* name) {
  gf_validate(cmd);
  gf_validate(!gf_strnull(name));

  _(gf_strassign(&cmd->name, name));
  
  return GF_SUCCESS;
}

gf_status
gf_command_set_description(gf_command* cmd, const char* description) {
  gf_validate(cmd);
  gf_validate(!gf_strnull(description));

  _(gf_strassign(&cmd->description, description));

  return GF_SUCCESS;
}

void
gf_command_free(gf_command* cmd) {
  if (cmd) {
    if (cmd->free) {
      (void)cmd->free(cmd);
    }
  }
}

gf_status
gf_command_execute(gf_command* cmd) {
  gf_validate(cmd);

  if (!cmd->execute) {
    gf_raise(GF_E_EXEC, "no execution procedure.");
  }
  _(cmd->execute(cmd));
  
  return GF_SUCCESS;
}

/* -------------------------------------------------------------------------- */

#ifndef COMMAND_FACTORY_SIZE
#define COMMAND_FACTORY_SIZE 16
#endif  /* COMMAND_FACTORY_SIZE */

struct command_factory {
  gf_command_index* entries;
  gf_size_t used;
  gf_size_t size;
};
typedef struct command_factory command_factory;


static command_factory factory_ = { 0 };

/*!
** @brief Returns GF_TRUE if the command factory is initialized
*/

static gf_bool
command_factory_is_initialized(void) {
  return factory_.size == 0 ? GF_FALSE : GF_TRUE;
}

gf_status
gf_command_factory_init(void) {
  gf_command_index* tmp = NULL;

  static const gf_size_t size = sizeof(*tmp) * COMMAND_FACTORY_SIZE;

  _(gf_malloc((gf_ptr* )&tmp, size));
  /* Initialize */
  for (gf_size_t i = 0; i < COMMAND_FACTORY_SIZE; i++) {
    tmp[i].name = NULL;
    tmp[i].create = NULL;
  }
  /* Assign */
  factory_.entries = tmp;
  factory_.used = 0;
  factory_.size = COMMAND_FACTORY_SIZE;

  return GF_SUCCESS;
}

void
gf_command_factory_clean(void) {
  if (factory_.entries) {
    for (gf_size_t i = 0; i < COMMAND_FACTORY_SIZE; i++) {
      if (factory_.entries[i].name) {
        gf_free(factory_.entries[i].name);
      }
      factory_.entries[i].name = NULL;
      factory_.entries[i].create = NULL;
    }
    gf_free(factory_.entries);
  }
  /* Assign */
  factory_.entries = NULL;
  factory_.used = 0;
  factory_.size = 0;
}

static gf_status
command_factory_add(const gf_command_index* entry) {
  gf_validate(entry);

  /* Auto initialization */
  if (!command_factory_is_initialized()) {
    _(gf_command_factory_init());
  }
  /* Extend the entries array */
  if (factory_.used >= factory_.size) {
    const gf_size_t new_size = factory_.size * 2;

    /* We don't shrink the entries array */
    assert(new_size > factory_.size);
    
    _(gf_realloc((gf_ptr* )&factory_.entries, new_size));
    factory_.size = new_size;
  }
  /* Register the command */
  _(gf_strdup(&factory_.entries[factory_.used].name, entry->name));
  factory_.entries[factory_.used].create = entry->create;
  factory_.used += 1;
  
  return GF_SUCCESS;
}

gf_status
gf_command_factory_add_commands(const gf_command_index* index, gf_size_t size) {
  gf_validate(index);

  for (gf_size_t i = 0; i < size; i++) {
    _(command_factory_add(&index[i]));
  }
  
  return GF_SUCCESS;
}

gf_status
gf_command_factory_show_helps(void) {
  for (gf_size_t i = 1; i < factory_.used; i++) {
    gf_command* cmd = NULL;

    _(gf_command_create(&cmd, factory_.entries[i].name));
    gf_msg("  %-16s %s", cmd->name, cmd->description);
    gf_command_free(cmd);
  }
  return GF_SUCCESS;
}


gf_status
gf_command_create(gf_command** cmd, const char* name) {
  gf_command* tmp = NULL;
  
  gf_validate(cmd);
  gf_validate(!gf_strnull(name));

  if (!command_factory_is_initialized()) {
    gf_raise(GF_E_STATE, "The command factory is not initialized.");
  }
  for (gf_size_t i = 0; i < factory_.used; i++) {
    if (!strcmp(factory_.entries[i].name, name)) {
      if (!factory_.entries[i].create) {
        gf_raise(GF_E_STATE, "Specified command is not runnable.");
      }
      _(factory_.entries[i].create(&tmp));
      *cmd = tmp;
      return GF_SUCCESS;
    }
  }
  /* No command has been found */
  gf_raise(GF_E_PARAM, "Unknown command '%s'", name);
}
