#include "argtable3.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>

enum {
  // ARGTABLE_ARG_MAX is the maximum number of arguments
  ARGTABLE_ARG_MAX = 20
};

/* global arg_xxx structs */
struct arg_lit *help, *version;
struct arg_lit *vrb;
struct arg_end *end;

int main(int argc, char **argv) {
  // the global arg_xxx structs are initialised within the argtable
  void *argtable[] = {
      help = arg_litn(NULL, "help", 0, 1, "display this help and exit"),
      version =
          arg_litn(NULL, "version", 0, 1, "display version info and exit"),
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
    goto exit;
  }

  // If the parser returned any errors then display them and exit
  if (nerrors > 0) {
    // Display the error details contained in the arg_end struct.
    arg_print_errors(stdout, end, progname);
    printf("Try '%s --help' for more information.\n", progname);
    exitcode = 1;
    goto exit;
  }

  // Set verbosity level
  log_set_level(LOG_INFO);
  if (vrb->count > 0) {
    log_set_level(LOG_TRACE);
  }

  // Welcome
  log_info("Welcome to Gnaro!");

cleanup:
  // Clear resources (cgroups, stack, sockets)
  log_info("freeing resources...");

exit:
  log_debug("freeing argtable...");
  arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
  log_info("so long and thanks for all the wasps!");
  return exitcode;
}
