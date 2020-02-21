/*-
 * This file is part of Grayfish project. For license details, see the file
 * 'LICENSE.md' in this package.
 */
/*!
** @file gf/main.c
** @brief The entry point of Grayfish.
*/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <libgf/libgf.h>

/* -------------------------------------------------------------------------- */

/*!
** @brief Output raw message on the console.
**
**
**
*/

void
gfc_raw_msg(const char* fmt, ...) {
  va_list args = { 0 };

  va_start(args, fmt);
  fprintf(stdout, fmt, args);
  va_end(args);
}

#define gfc_error(code, msg, ...)              \
  do {                                         \
    gfc_raw_msg("error: " msg, ##__VA_ARGS__); \
    return (code);                             \
  } while (0);

/* -------------------------------------------------------------------------- */

gf_status
gfc_stdout_open(gf_ptr* ptr, const char* dst) {
  FILE** fp = NULL;
  
  (void)dst;

  if (!ptr) {
    gfc_error(GF_E_PARAM, "Failed to open stdout stream.");
  }
  fp = (FILE**)ptr;
  *fp = stdout;

  return GF_SUCCESS;
}

void
gfc_stdout_close(gf_ptr* ptr) {
  if (ptr) {
    *ptr = NULL;
  }
}

int
gfc_stdout_write(gf_ptr ptr, const char* fmt, va_list args) {
  FILE* fp = (FILE*)ptr;

  if (!fp || !fmt) {
    return -1;
  }

  return vfprintf(fp, fmt, args);
}

/* -------------------------------------------------------------------------- */

gf_status
gfc_add_stdout_stream(void) {
  gf_status rc = 0;
  gf_write_stream* stream = NULL;

  rc = gf_stream_new(
    &stream, gfc_stdout_open, gfc_stdout_close, gfc_stdout_write);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  rc = gf_log_add_stream(stream);
  if (rc != GF_SUCCESS) {
    gf_stream_free(stream);
    return rc;
  }
  rc = gf_stream_open(stream, NULL);
  if (rc != GF_SUCCESS) {
    gf_stream_free(stream);
    return rc;
  }
  
  return GF_SUCCESS;
}

void
gfc_set_stream_buffer(void) {
  /* Set streams to unbuffered  */
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
}

gf_status
gfc_read_config(void) {
  gf_status rc = 0;
  gf_path* path = 0;

  rc = gf_system_get_system_config_file_path(&path);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  rc = gf_config_read_file(path);
  gf_path_free(path);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  
  return GF_SUCCESS;
}

gf_status
gfc_init_process(void) {
  gf_status rc = 0;
  
  gfc_set_stream_buffer();

  rc = gfc_add_stdout_stream();
  if (rc != GF_SUCCESS) {
    return rc;
  }

  rc = gfc_read_config();
  if (rc != GF_SUCCESS) {
    return rc;
  }

  return GF_SUCCESS;
}

/*!
** @brief Process main
**
** @param [in] argc Count of the command argument vector
** @param [in] argv The command argcument vector
**
** @return Returns GF_SUCCESS on success, GF_E_* otherwise.
*/

gf_status
gfc_main_process(int argc, char *argv[]) {
  gf_status rc = 0;
  gf_cmd_base *cmd = NULL;

  rc = gfc_init_process();
  if (rc != GF_SUCCESS) {
    return rc;
  }
  
  rc = gf_cmd_base_create(&cmd, "main");
  if (rc != GF_SUCCESS) {
    return rc;
  }
  rc = gf_cmd_base_set_args(cmd, &argc, &argv);
  if (rc != GF_SUCCESS) {
    gf_cmd_base_free(cmd);
    return rc;
  }
  /*
  ** Consume the command argument argv[0].
  ** The argv[0] represents the executalbe command module, which is not needed
  ** in this module.
  */
  rc = gf_cmd_base_consume_args(cmd, NULL);
  if (rc != GF_SUCCESS) {
    gf_cmd_base_free(cmd);
    return rc;
  }
  rc = gf_cmd_base_execute(cmd);
  gf_cmd_base_free(cmd);
  if (rc != GF_SUCCESS) {
    return rc;
  }
  
  return GF_SUCCESS;
}

/* -------------------------------------------------------------------------- */

/*!
** @brief Show message at the beggining of the process.
*/

void
show_prologue_message() {
  /* Do nothing fow now */
}

/*!
** @brief Show message at the end of the process.
*/

void
show_epilogue_message() {
  /* Do nothing fow now */
}

/*!
** @brief The entry point.
**
** @param [in] argc Count of the command argument vector
** @param [in] argv The command argcument vector
**
** @return EXIT_SUCCESS on success, EXIT_FAILURE on error
*/

int
main(int argc, char *argv[]) {
  gf_status rc = 0;

  show_prologue_message();
  rc = gfc_main_process(argc, argv);
  show_epilogue_message();
  
  return (rc == GF_SUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE;
}
