#include "meta.h"
#include "input.h"
#include "log.h"
#include <string.h>

// Execute a meta command (e.g. .exit)
MetaCommandResult meta_execute_command(InputBuffer *input_buffer) {
  log_debug("executing meta command '%s'...", input_buffer->buffer);

  // .exit lets gnaro know that we want to exit the program
  if (strcmp(input_buffer->buffer, ".exit") == 0) {
    return META_COMMAND_EXIT;
  }

  log_warn("unrecognized meta command '%s'", input_buffer->buffer);
  return META_COMMAND_UNRECOGNIZED;
}
