#include "../include/database.h"
#include "../include/input.h"
#include "../include/meta.h"
#include "../include/statement.h"
#include "../lib/argtable/argtable3.h"
#include "../lib/log/log.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
  // ARGTABLE_ARG_MAX is the maximum number of arguments
  ARGTABLE_ARG_MAX = 20
};

struct arg_lit *help, *version;
struct arg_str *dbf;
struct arg_lit *vrb;
struct arg_end *end;

int main(int argc, char **argv) {
  // the global arg_xxx structs are initialised within the argtable
  void *argtable[] = {
      help = arg_litn(NULL, "help", 0, 1, "display this help and exit"),
      version =
          arg_litn(NULL, "version", 0, 1, "display version info and exit"),
      dbf =
          arg_strn("d", "database", "<string>", 1, 1, "path to database file"),
      vrb = arg_litn("v", "verbosity", 0, 1, "verbose output"),
      end = arg_end(ARGTABLE_ARG_MAX),
  };

  int exitcode = 0;
  char progname[] = "gnaro";

  int nerrors;
  nerrors = arg_parse(argc, argv, argtable);

  // special case: '--help' takes precedence over error reporting
  if (help->count > 0) {
    printf("Usage: %s", progname);
    arg_print_syntax(stdout, argtable, "\n");
    arg_print_glossary(stdout, argtable, "  %-25s %s\n");
    exitcode = 0;
    goto exithard;
  }

  // If the parser returned any errors then display them and exit
  if (nerrors > 0) {
    // Display the error details contained in the arg_end struct.
    arg_print_errors(stdout, end, progname);
    printf("Try '%s --help' for more information.\n", progname);
    exitcode = 1;
    goto exithard;
  }

  // Set verbosity level
  log_set_level(LOG_INFO);
  if (vrb->count > 0) {
    log_set_level(LOG_TRACE);
  }

  log_debug("starting gnaro repl...");

  InputBuffer *input_buffer = input_new_buffer();
  Database *database = database_open(dbf->sval[0]);
  if (database == NULL) {
    log_error("failed to open database file %s", dbf->sval[0]);
    exitcode = 1;
    goto cleanup;
  }

  // Start REPL loop
  while (true) {
    printf("gnaro> ");

    if (input_read(input_buffer) < 0) {
      log_error("error reading input");
      goto cleanup;
    }

    // Meta commands start with a '.'
    if (input_buffer->buffer[0] == '.') {
      switch (meta_execute_command(input_buffer->buffer, database)) {
      case (META_COMMAND_SUCCESS):
        continue;
      case (META_COMMAND_EXIT):
        goto cleanup;
      case (META_COMMAND_UNRECOGNIZED):
        log_error("unrecognized command '%s'.", input_buffer->buffer);
        continue;
      }
    }

    // Prepare statement for SQL operations
    log_debug("preparing statement...");
    Statement statement;
    switch (statement_prepare(input_buffer->buffer, &statement)) {
    case (STATEMENT_PREPARE_SUCCESS):
      log_debug("statement prepared...");
      break;
    case (STATEMENT_PREPARE_NEGATIVE_ID):
      log_error("failed to prepare statement: id must be greater than zero.");
      continue;
    case (STATEMENT_PREPARE_STRING_TOO_LONG):
      log_error("failed to prepare statement: string is too long.");
      continue;
    case (STATEMENT_PREPARE_SYNTAX_ERROR):
      log_error("failed to prepare statement: syntax error.");
      continue;
    case (STATEMENT_PREPARE_UNRECOGNIZED):
      log_error(
          "failed to prepare statement: unrecognized keyword at start of '%s'.",
          input_buffer->buffer);
      continue;
    }

    // Execute statement
    log_debug("executing statement...");
    switch (statement_execute(&statement, database)) {
    case (STATEMENT_EXECUTE_SUCCESS):
      log_info("statement executed");
      break;
    case (STATEMENT_EXECUTE_TABLE_FULL):
      log_error("statement failed: database full.");
      break;
    case (STATEMENT_EXECUTE_DUPLICATE_KEY):
      log_error("statement failed: duplicate key.");
      break;
    default:
      log_error("unknown error executing statement.");
      break;
    }
  }

cleanup:
  // Clear resources (cgroups, stack, sockets)
  log_info("freeing resources...");
  log_info("freeing input buffer...");
  input_close_buffer(input_buffer);
  log_info("freeing database...");
  if (database_close(database) == DATABASE_CLOSE_FAIL) {
    log_error("failed to close database");
  }
  log_debug("freeing argtable...");
  log_info("so long and thanks for all the wasps!");

exithard:
  arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
  return exitcode;
}
