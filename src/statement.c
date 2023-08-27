#include "statement.h"
#include "btree.h"
#include "cursor.h"
#include "database.h"
#include "input.h"
#include "log.h"
#include "pager.h"
#include "row.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Convert user input into an internal representation
// statement_prepare roughly corresponds to SQL Command Processor in SQLite
StatementPrepareResult statement_prepare(char *query, Statement *statement) {

  // strncmp for "insert" since the keyword will be followed by data (e.g.
  // insert 1 lucavallin foo@bar.com)
  if (strncmp(query, "insert", STATEMENT_INSERT_COMMAND_SIZE) == 0) {
    log_debug("preparing insert statement...");
    return statement_prepare_insert(query, statement);
  }
  if (strcmp(query, "select") == 0) {
    log_debug("preparing select statement...");
    statement->type = STATEMENT_SELECT;
    return STATEMENT_PREPARE_SUCCESS;
  }

  log_warn("could not recognize statement...");
  return STATEMENT_PREPARE_UNRECOGNIZED;
}

StatementPrepareResult statement_prepare_insert(char *query,
                                                Statement *statement) {
  statement->type = STATEMENT_INSERT;

  log_debug("parsing insert statement...");
  char *keyword = strtok(query, " ");
  char *id_string = strtok(NULL, " ");
  char *username = strtok(NULL, " ");
  char *email = strtok(NULL, " ");

  if (id_string == NULL || username == NULL || email == NULL) {
    return STATEMENT_PREPARE_SYNTAX_ERROR;
  }

  log_debug("requested statement: %s %s %s %s...", keyword, id_string, username,
            email);
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
StatementExecuteResult statement_execute(Statement *statement,
                                         Database *database) {
  switch (statement->type) {
  case (STATEMENT_INSERT):
    log_debug("requested insert statement...");
    return statement_execute_insert(statement, database);
    break;
  case (STATEMENT_SELECT):
    log_debug("requested select statement...");
    return statement_execute_select(database);
    break;
  default:
    log_error("unknown statement type");
    break;
  }
}

StatementExecuteResult statement_execute_insert(Statement *statement,
                                                Database *database) {
  log_debug("executing insert statement...");
  void *node = pager_get_page(database->pager, database->root_page_num);
  uint32_t num_cells = (*btree_node_leaf_num_cells(node));

  log_debug("serializing row for insert...");
  Row *row_to_insert = &(statement->row_to_insert);
  uint32_t key_to_insert = row_to_insert->id;
  Cursor *cursor = cursor_find_key(database, key_to_insert);

  if (cursor->cell_num < num_cells) {
    uint32_t key_at_index = *btree_node_leaf_key(node, cursor->cell_num);
    if (key_at_index == key_to_insert) {
      return STATEMENT_EXECUTE_DUPLICATE_KEY;
    }
  }

  btree_node_leaf_insert(cursor, row_to_insert->id, row_to_insert);

  cursor_close(cursor);

  log_debug("inserted row %d", row_to_insert->id);
  return STATEMENT_EXECUTE_SUCCESS;
}

StatementExecuteResult statement_execute_select(Database *database) {
  log_debug("executing select statement...");
  log_debug("getting cursor at end of database...");
  Cursor *cursor = cursor_start(database);

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
