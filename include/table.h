#ifndef TABLE_H
#define TABLE_H

#include "pager.h"
#include <stdint.h>

// Table is a struct that represents a table. It uses a pager to persist data.
// It roughly corresponds to a B-Tree in SQLite.
//
// Data will be stored as follows:
// - Store rows in blocks of memory called pages
// - Each page stores as many rows as it can fit
// - Rows are serialized into a compact representation with each page
// - Pages are only allocated as needed
// - Keep a fixed-size array of pointers to pages
typedef struct {
  Pager *pager;
  uint32_t root_page_num;
} Table;

// Opens a connection to a database.
Table *table_db_open(const char *filename);

// Closes a connection to a database.
void table_db_close(Table *table);

#endif
