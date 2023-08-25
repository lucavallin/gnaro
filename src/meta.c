#include "meta.h"
#include "input.h"
#include "log.h"
#include "node.h"
#include "row.h"
#include "table.h"
#include <stdio.h>
#include <string.h>

// Execute a meta command (e.g. .exit)
MetaCommandResult meta_execute_command(InputBuffer *input_buffer,
                                       Table *table) {
  log_debug("executing meta command '%s'...", input_buffer->buffer);

  // .exit lets gnaro know that we want to exit the program
  if (strcmp(input_buffer->buffer, ".exit") == 0) {
    return META_COMMAND_EXIT;
  } else if (strcmp(input_buffer->buffer, ".btree") == 0) {
    printf("Tree:\n");
    node_print_tree(table->pager, 0, 0);
    return META_COMMAND_SUCCESS;
  } else if (strcmp(input_buffer->buffer, ".constants") == 0) {
    printf("Constants:\n");
    printf("ROW_SIZE: %d\n", ROW_SIZE);
    printf("NODE_COMMON_HEADER_SIZE: %d\n", NODE_COMMON_HEADER_SIZE);
    printf("NODE_LEAF_HEADER_SIZE: %d\n", NODE_LEAF_HEADER_SIZE);
    printf("NODE_LEAF_CELL_SIZE: %d\n", NODE_LEAF_CELL_SIZE);
    printf("NODE_LEAF_SPACE_FOR_CELLS: %d\n", NODE_LEAF_SPACE_FOR_CELLS);
    printf("NODE_LEAF_MAX_CELLS: %d\n", NODE_LEAF_MAX_CELLS);

    return META_COMMAND_SUCCESS;
  }

  log_warn("unrecognized meta command '%s'", input_buffer->buffer);
  return META_COMMAND_UNRECOGNIZED;
}
