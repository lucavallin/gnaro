#include "statement.h"
#include "input.h"
#include "log.h"
#include "row.h"
#include "table.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Convert user input into an internal representation
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
  if (table->num_rows >= TABLE_MAX_ROWS) {
    log_warn("table is full");
    return STATEMENT_EXECUTE_TABLE_FULL;
  }

  log_debug("serializing row for insert...");
  Row *row_to_insert = &(statement->row_to_insert);
  row_serialize(row_to_insert, table_row_slot(table, table->num_rows));
  table->num_rows += 1;

  log_debug("inserted row %d", row_to_insert->id);
  return STATEMENT_EXECUTE_SUCCESS;
}

StatementExecuteResult statement_execute_select(Statement *statement,
                                                Table *table) {
  Row row;
  log_debug("executing select statement...");

  // Scan through all rows in the table
  log_debug("scanning table...");
  for (uint32_t i = 0; i < table->num_rows; i++) {
    row_deserialize(table_row_slot(table, i), &row);
    // TODO: move to main?
    row_print(&row);
  }

  log_debug("select statement executed");
  return STATEMENT_EXECUTE_SUCCESS;
}
