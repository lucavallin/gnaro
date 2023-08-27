#ifndef STATEMENT_H
#define STATEMENT_H

#include "database.h"
#include "row.h"

enum {
  // STATEMENT_INSERT_COMMAND_SIZE is the length of the "insert" command.
  STATEMENT_INSERT_COMMAND_SIZE = 6
};

// StatementPrepareResult is an enum that represents the result of preparing a
// statement.
typedef enum {
  STATEMENT_PREPARE_SUCCESS,
  STATEMENT_PREPARE_STRING_TOO_LONG,
  STATEMENT_PREPARE_NEGATIVE_ID,
  STATEMENT_PREPARE_SYNTAX_ERROR,
  STATEMENT_PREPARE_UNRECOGNIZED,
} StatementPrepareResult;

// StatementExecuteResult is an enum that represents the result of executing a
// statement.
typedef enum {
  STATEMENT_EXECUTE_SUCCESS,
  STATEMENT_EXECUTE_TABLE_FULL,
  STATEMENT_EXECUTE_DUPLICATE_KEY,
} StatementExecuteResult;

// StatementType is an enum that represents the type of a statement.
typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } StatementType;

// Statement is a struct that represents a statement.
typedef struct {
  StatementType type;
  // Only used by insert statement
  Row row_to_insert;
} Statement;

// statement_prepare prepares a statement.
StatementPrepareResult statement_prepare(char *query, Statement *statement);

// statement_prepare_insert prepares an insert statement.
StatementPrepareResult statement_prepare_insert(char *query,
                                                Statement *statement);

// statement_execute executes a statement.
StatementExecuteResult statement_execute(Statement *statement,
                                         Database *database);

// statement_execute_insert executes an insert statement.
StatementExecuteResult statement_execute_insert(Statement *statement,
                                                Database *database);

// statement_execute_select executes a select statement.
StatementExecuteResult statement_execute_select(Database *database);

#endif
