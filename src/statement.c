#include "statement.h"
#include "input.h"
#include "log.h"
#include <string.h>

// Convert user input into an internal representation
StatementPrepareResult statement_prepare(InputBuffer *input_buffer,
                                         Statement *statement) {

  // strncmp for "insert" since the keyword will be followed by data (e.g.
  // insert 1 lucavallin foo@bar.com)
  if (strncmp(input_buffer->buffer, "insert", STATEMENT_INSERT_COMMAND_SIZE) ==
      0) {
    log_debug("preparing insert statement...");
    statement->type = STATEMENT_INSERT;
    return STATEMENT_PREPARE_SUCCESS;
  }
  if (strcmp(input_buffer->buffer, "select") == 0) {
    log_debug("preparing select statement...");
    statement->type = STATEMENT_SELECT;
    return STATEMENT_PREPARE_SUCCESS;
  }

  log_warn("could not recognize statement...");
  return STATEMENT_PREPARE_UNRECOGNIZED;
}

// Execute the statement
void statement_execute(Statement *statement) {
  switch (statement->type) {
  case (STATEMENT_INSERT):
    log_debug("executing insert statement...");
    break;
  case (STATEMENT_SELECT):
    log_debug("executing update statement...");
    break;
  default:
    log_error("unknown statement type");
    break;
  }
}
