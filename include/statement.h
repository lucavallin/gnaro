#ifndef STATEMENT_H
#define STATEMENT_H

#include "input.h"
#include "row.h"
#include "table.h"

enum {
  // STATEMENT_INSERT_COMMAND_SIZE is the length of the "insert" command.
  STATEMENT_INSERT_COMMAND_SIZE = 6
};

// StatementPrepareResult is an enum that represents the result of preparing a
// statement.
typedef enum {
  STATEMENT_PREPARE_SUCCESS,
  STATEMENT_PREPARE_UNRECOGNIZED,
  STATEMENT_PREPARE_SYNTAX_ERROR,
} StatementPrepareResult;

// StatementExecuteResult is an enum that represents the result of executing a
// statement.
typedef enum {
  STATEMENT_EXECUTE_SUCCESS,
  STATEMENT_EXECUTE_TABLE_FULL,
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
StatementPrepareResult statement_prepare(InputBuffer *input_buffer,
                                         Statement *statement);

// statement_execute executes a statement.
StatementExecuteResult statement_execute(Statement *statement, Table *table);

// statement_execute_insert executes an insert statement.
StatementExecuteResult statement_execute_insert(Statement *statement,
                                                Table *table);

// statement_execute_select executes a select statement.
StatementExecuteResult statement_execute_select(Statement *statement,
                                                Table *table);

#endif
