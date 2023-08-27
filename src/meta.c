#include "meta.h"
#include "database.h"
#include "input.h"
#include "log.h"
#include "node.h"
#include <string.h>

// Execute a meta command (e.g. .exit)
MetaCommandResult meta_execute_command(InputBuffer *input_buffer,
                                       Database *database) {
  log_debug("executing meta command '%s'...", input_buffer->buffer);

  // .exit lets gnaro know that we want to exit the program
  if (strcmp(input_buffer->buffer, ".exit") == 0) {
    return META_COMMAND_EXIT;
  }

  if (strcmp(input_buffer->buffer, ".btree") == 0) {
    log_info("printing tree...");
    node_print_tree(database->pager, 0, 0);
    return META_COMMAND_SUCCESS;
  }

  log_warn("unrecognized meta command '%s'", input_buffer->buffer);
  return META_COMMAND_UNRECOGNIZED;
}
