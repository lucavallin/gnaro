#ifndef CURSOR_H
#define CURSOR_H

#include "database.h"
#include <stdbool.h>
#include <stdint.h>

// Cursor represents a position in a database
typedef struct {
  Database *database;
  uint32_t page_num;
  uint32_t cell_num;
  // Indicates a position past the end of the database where a new row would be
  // inserted
  bool end_of_table;
} Cursor;

// Create a cursor at the beginning of the database
Cursor *cursor_start(Database *database);

// Find the position of the given key or where it should be inserted if it is
// not present
Cursor *cursor_find_key(Database *database, uint32_t key);

// Move a cursor to the next row
void cursor_advance(Cursor *cursor);

// Handle memory I/O for a particular row.
void *cursor_value(Cursor *cursor);

// Free a cursor
void cursor_close(Cursor *cursor);

#endif
