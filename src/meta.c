#include "../include/meta.h"
#include "../include/btree.h"
#include "../include/database.h"
#include "../lib/log/log.h"
#include <string.h>

// Execute a meta command (e.g. .exit)
MetaCommandResult meta_execute_command(char *command, Database *database) {
  log_debug("executing meta command '%s'...", command);

  // .exit lets gnaro know that we want to exit the program
  if (strcmp(command, ".exit") == 0) {
    return META_COMMAND_EXIT;
  }

  if (strcmp(command, ".btree") == 0) {
    log_info("printing tree...");
    btree_print(database->pager, 0, 0);
    return META_COMMAND_SUCCESS;
  }

  log_warn("unrecognized meta command '%s'", command);
  return META_COMMAND_UNRECOGNIZED;
}
