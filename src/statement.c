#include "statement.h"
#include "cursor.h"
#include "input.h"
#include "log.h"
#include "node.h"
#include "pager.h"
#include "row.h"
#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Convert user input into an internal representation
// statement_prepare roughly corresponds to SQL Command Processor in SQLite
StatementPrepareResult statement_prepare(InputBuffer *input_buffer,
                                         Statement *statement) {

  // strncmp for "insert" since the keyword will be followed by data (e.g.
  // insert 1 lucavallin foo@bar.com)
  if (strncmp(input_buffer->buffer, "insert", STATEMENT_INSERT_COMMAND_SIZE) ==
      0) {
    log_debug("preparing insert statement...");
    return statement_prepare_insert(input_buffer, statement);
  }
  if (strcmp(input_buffer->buffer, "select") == 0) {
    log_debug("preparing select statement...");
    statement->type = STATEMENT_SELECT;
    return STATEMENT_PREPARE_SUCCESS;
  }

  log_warn("could not recognize statement...");
  return STATEMENT_PREPARE_UNRECOGNIZED;
}

StatementPrepareResult statement_prepare_insert(InputBuffer *input_buffer,
                                                Statement *statement) {
  statement->type = STATEMENT_INSERT;

  log_debug("parsing insert statement...");
  char *keyword = strtok(input_buffer->buffer, " ");
  char *id_string = strtok(NULL, " ");
  char *username = strtok(NULL, " ");
  char *email = strtok(NULL, " ");

  if (id_string == NULL || username == NULL || email == NULL) {
    return STATEMENT_PREPARE_SYNTAX_ERROR;
  }

  log_debug("validating insert statement...");
  int id = atoi(id_string);
  if (id < 0) {
    return STATEMENT_PREPARE_NEGATIVE_ID;
  }
  if (strlen(username) > ROW_COLUMN_USERNAME_SIZE) {
    return STATEMENT_PREPARE_STRING_TOO_LONG;
  }
  if (strlen(email) > ROW_COLUMN_EMAIL_SIZE) {
    return STATEMENT_PREPARE_STRING_TOO_LONG;
  }

  statement->row_to_insert.id = id;
  strcpy(statement->row_to_insert.username, username);
  strcpy(statement->row_to_insert.email, email);

  log_debug("insert statement prepared");
  return STATEMENT_PREPARE_SUCCESS;
}

// Execute the statement
// statement_execute roughly corresponds to the Virtual Machine in SQLite
StatementExecuteResult statement_execute(Statement *statement, Table *table) {
  switch (statement->type) {
  case (STATEMENT_INSERT):
    log_debug("requested insert statement...");
    return statement_execute_insert(statement, table);
    break;
  case (STATEMENT_SELECT):
    log_debug("requested select statement...");
    return statement_execute_select(statement, table);
    break;
  default:
    log_error("unknown statement type");
    break;
  }
}

StatementExecuteResult statement_execute_insert(Statement *statement,
                                                Table *table) {
  log_debug("executing insert statement...");
  void *node = pager_get_page(table->pager, table->root_page_num);
  if ((*node_leaf_num_cells(node) >= LEAF_NODE_MAX_CELLS)) {
    log_warn("table is full");
    return STATEMENT_EXECUTE_TABLE_FULL;
  }

  log_debug("serializing row for insert...");
  Row *row_to_insert = &(statement->row_to_insert);
  Cursor *cursor = cursor_at_end(table);
  node_leaf_insert(cursor, row_to_insert->id, row_to_insert);

  cursor_close(cursor);

  log_debug("inserted row %d", row_to_insert->id);
  return STATEMENT_EXECUTE_SUCCESS;
}

StatementExecuteResult statement_execute_select(Statement *statement,
                                                Table *table) {
  log_debug("executing select statement...");
  log_debug("getting cursor at end of table...");
  Cursor *cursor = cursor_at_start(table);

  Row row;
  while (!(cursor->end_of_table)) {
    log_debug("deserializing row...");
    row_deserialize(cursor_value(cursor), &row);
    row_print(&row);
    log_debug("advancing cursor...");
    cursor_advance(cursor);
  }

  log_debug("closing cursor...");
  cursor_close(cursor);

  log_debug("select statement executed");
  return STATEMENT_EXECUTE_SUCCESS;
}
