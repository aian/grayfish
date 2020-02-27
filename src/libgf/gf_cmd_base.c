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
gf_cmd_base_init(gf_cmd_base* cmd) {
  gf_validate(cmd);

  cmd->name = NULL;
  cmd->description = NULL;
  cmd->args = NULL;
  cmd->free = NULL;
  cmd->execute = NULL;

  return GF_SUCCESS;
}

gf_status
gf_cmd_base_prepare(gf_cmd_base* cmd) {
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
gf_cmd_base_clear(gf_cmd_base* cmd) {
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
    (void)gf_cmd_base_init(cmd);
  }
}

gf_status
gf_cmd_base_set_info(gf_cmd_base* cmd, const gf_cmd_base_info* info) {
  gf_validate(cmd);
  gf_validate(info);

  _(gf_cmd_base_prepare(cmd));
  _(gf_cmd_base_set_name(cmd, info->base.name));
  _(gf_cmd_base_set_description(cmd, info->base.description));
  _(gf_cmd_base_add_options(cmd, info->options));

  cmd->create = info->base.create;
  cmd->free = info->base.free;
  cmd->execute = info->base.execute;
  
  return GF_SUCCESS;
}


gf_status
gf_cmd_base_add_options(gf_cmd_base* cmd, const gf_option* options) {
  gf_validate(cmd);
  gf_validate(options);

  _(gf_args_add_option_table(cmd->args, options));
  
  return GF_SUCCESS;
}

gf_status
gf_cmd_base_set_args(gf_cmd_base* cmd, int* argc,  char*** argv) {
  gf_validate(cmd);
  gf_validate(argc);
  gf_validate(argv);

  _(gf_args_set(cmd->args, argc, argv));

  return GF_SUCCESS;
}

gf_status
gf_cmd_base_inherit_args(gf_cmd_base* dst, const gf_cmd_base* src) {
  gf_validate(dst);
  gf_validate(src);
  gf_validate(dst->args);
  gf_validate(src->args);

  _(gf_args_inherit(dst->args, src->args));

  return GF_SUCCESS;
}

gf_status
gf_cmd_base_consume_args(gf_cmd_base* cmd, char** str) {
  gf_validate(cmd);

  if (!cmd->args) {
    gf_debug("cmd->args is NULL.");
    gf_raise(GF_E_STATE, "Command argument is not set.");
  }
  _(gf_args_consume(cmd->args, str));
  
  return GF_SUCCESS;
}

gf_status
gf_cmd_base_set_name(gf_cmd_base* cmd, const char* name) {
  gf_validate(cmd);
  gf_validate(!gf_strnull(name));

  _(gf_strassign(&cmd->name, name));
  
  return GF_SUCCESS;
}

gf_status
gf_cmd_base_set_description(gf_cmd_base* cmd, const char* description) {
  gf_validate(cmd);
  gf_validate(!gf_strnull(description));

  _(gf_strassign(&cmd->description, description));

  return GF_SUCCESS;
}

void
gf_cmd_base_free(gf_cmd_base* cmd) {
  if (cmd) {
    if (cmd->free) {
      (void)cmd->free(cmd);
    }
  }
}

gf_status
gf_cmd_base_execute(gf_cmd_base* cmd) {
  gf_validate(cmd);

  if (!cmd->execute) {
    gf_raise(GF_E_EXEC, "no execution procedure.");
  }
  _(cmd->execute(cmd));
  
  return GF_SUCCESS;
}

/* -------------------------------------------------------------------------- */

#ifndef CMD_FACTORY_SIZE
#define CMD_FACTORY_SIZE 16
#endif  /* CMD_FACTORY_SIZE */

struct cmd_factory {
  gf_cmd_index* entries;
  gf_size_t used;
  gf_size_t size;
};
typedef struct cmd_factory cmd_factory;


static cmd_factory factory_ = { 0 };

/*!
** @brief Returns GF_TRUE if the command factory is initialized
*/

static gf_bool
cmd_factory_is_initialized(void) {
  return factory_.size == 0 ? GF_FALSE : GF_TRUE;
}

gf_status
gf_cmd_factory_init(void) {
  gf_cmd_index* tmp = NULL;

  static const gf_size_t size = sizeof(*tmp) * CMD_FACTORY_SIZE;

  _(gf_malloc((gf_ptr* )&tmp, size));
  /* Initialize */
  for (gf_size_t i = 0; i < CMD_FACTORY_SIZE; i++) {
    tmp[i].name = NULL;
    tmp[i].create = NULL;
  }
  /* Assign */
  factory_.entries = tmp;
  factory_.used = 0;
  factory_.size = CMD_FACTORY_SIZE;

  return GF_SUCCESS;
}

void
gf_cmd_factory_clean(void) {
  if (factory_.entries) {
    for (gf_size_t i = 0; i < CMD_FACTORY_SIZE; i++) {
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
cmd_factory_add(const gf_cmd_index* entry) {
  gf_validate(entry);

  /* Auto initialization */
  if (!cmd_factory_is_initialized()) {
    _(gf_cmd_factory_init());
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
gf_cmd_factory_add_commands(const gf_cmd_index* index, gf_size_t size) {
  gf_validate(index);

  for (gf_size_t i = 0; i < size; i++) {
    _(cmd_factory_add(&index[i]));
  }
  
  return GF_SUCCESS;
}

gf_status
gf_cmd_factory_show_helps(void) {
  for (gf_size_t i = 1; i < factory_.used; i++) {
    gf_cmd_base* cmd = NULL;

    _(gf_cmd_create(&cmd, factory_.entries[i].name));
    gf_msg("  %-16s %s", cmd->name, cmd->description);
    gf_cmd_base_free(cmd);
  }
  return GF_SUCCESS;
}


gf_status
gf_cmd_create(gf_cmd_base** cmd, const char* name) {
  gf_cmd_base* tmp = NULL;
  
  gf_validate(cmd);
  gf_validate(!gf_strnull(name));

  if (!cmd_factory_is_initialized()) {
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
