/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file libgf/gf_global.c
** @brief Library global settings.
*/
#include <stdio.h>
#include <stdlib.h>

#include <windows.h>

#include <libxml/parser.h>
#include <libxslt/xslt.h>
#include <libxslt/extensions.h>
#include <libexslt/exslt.h>

#include <libgf/gf_countof.h>
#include <libgf/gf_log.h>
#include <libgf/gf_config.h>

#include <libgf/gf_cmd_base.h>
#include <libgf/gf_cmd_main.h>
#include <libgf/gf_cmd_help.h>
#include <libgf/gf_cmd_version.h>
#include <libgf/gf_cmd_setup.h>
#include <libgf/gf_cmd_update.h>
#include <libgf/gf_cmd_build.h>
#include <libgf/gf_cmd_clean.h>
#include <libgf/gf_cmd_list.h>
#include <libgf/gf_cmd_serve.h>

#include <libgf/gf_global.h>

#include <libgf/gf_local.h>

static const gf_cmd_base_index command_index_[] = {
  { "main",    gf_main_new    },
  { "help",    gf_help_new    },
  { "version", gf_version_new },
  { "setup",   gf_setup_new,  },
  { "config",  gf_config_new, },
  { "update",  gf_update_new, },
  { "build",   gf_build_new,  },
  { "clean",   gf_clean_new,  },
  { "list",    gf_list_new,   },
  { "serve",   gf_serve_new,  },
};

static gf_status
register_commands(void) {
  _(gf_cmd_factory_add_commands(command_index_, gf_countof(command_index_)));
  return GF_SUCCESS;
}

gf_status
gf_global_init(void) {
  gf_status rc = 0;
  
  /* Logger initialization */
  gf_log_init();
  /* Setup for LibXML2 */
  xmlSubstituteEntitiesDefault(1);
  xmlLoadExtDtdDefaultValue = 1;
  xmlKeepBlanksDefault(0);
  /* Setup for LibXSLT */
  xsltRegisterTestModule();
  /* Load the LibEXSLT library */
  exsltRegisterAll();
  /* Register all of the command entry */
  register_commands();
  /* Setup internal configuration */
  rc = gf_config_init();
  if (rc != GF_SUCCESS) {
    gf_global_clean();
    gf_raise(GF_E_CONFIG, "Failed to init config.");
  }

  return GF_SUCCESS;
}

gf_status
gf_global_clean(void) {
  gf_status rc = 0;
  
  /* Clean the command factory registory */
  gf_cmd_factory_clean();
  /* Finalize the XML/XSLT libraries */
  xsltCleanupGlobals();
  /* NOTE: xmlCleanupParser() does not clean up parser state and does not
  ** deallocate any document related memory. This function should be called when
  ** the process has finished using LibXML2 library.
  */
  xmlCleanupParser();
  /* Logger finalization */
  gf_log_init();
  /* Clean the internal configuration */
  rc = gf_config_clean();
  if (rc != GF_SUCCESS) {
    return rc;
  }

  return GF_SUCCESS;
}
