#ifndef CURSOR_H
#define CURSOR_H

#include "table.h"
#include <stdbool.h>
#include <stdint.h>

// Cursor represents a position in a table
typedef struct {
  Table *table;
  uint32_t row_num;
  // Indicates a position past the end of the table where a new row would be
  // inserted
  bool end_of_table;
} Cursor;

// Create a cursor at the beginning of the table
Cursor *cursor_at_start(Table *table);

// Create a cursor at the end of the table
Cursor *cursor_at_end(Table *table);

// Move a cursor to the next row
void cursor_advance(Cursor *cursor);

// Handle memory I/O for a particular row.
void *cursor_value(Cursor *cursor);

// Free a cursor
void cursor_close(Cursor *cursor);

#endif
