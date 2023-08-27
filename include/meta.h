#ifndef META_H
#define META_H

#include "database.h"

// MetaCommandResult is an enum that represents the result of executing a meta
// command.
typedef enum {
  META_COMMAND_SUCCESS,
  META_COMMAND_EXIT,
  META_COMMAND_UNRECOGNIZED
} MetaCommandResult;

// Execute a meta command (e.g. .exit) and return the result.
MetaCommandResult meta_execute_command(char *command, Database *database);

#endif
