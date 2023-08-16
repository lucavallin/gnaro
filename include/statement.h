#ifndef STATEMENT_H
#define STATEMENT_H

#include "input.h"

enum {
  // STATEMENT_INSERT_COMMAND_SIZE is the length of the "insert" command.
  STATEMENT_INSERT_COMMAND_SIZE = 6
};

// StatementPrepareResult is an enum that represents the result of preparing a
// statement.
typedef enum {
  STATEMENT_PREPARE_SUCCESS,
  STATEMENT_PREPARE_UNRECOGNIZED
} StatementPrepareResult;

// StatementType is an enum that represents the type of a statement.
typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } StatementType;

// Statement is a struct that represents a statement.
typedef struct {
  StatementType type;
} Statement;

// statement_prepare prepares a statement.
StatementPrepareResult statement_prepare(InputBuffer *input_buffer,
                                         Statement *statement);

// statement_execute executes a statement.
void statement_execute(Statement *statement);

#endif
