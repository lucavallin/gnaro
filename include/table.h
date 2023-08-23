#ifndef TABLE_H
#define TABLE_H

#include "pager.h"
#include "row.h"
#include <stdint.h>

// Table and rows configuration
static const uint32_t ROWS_PER_PAGE = PAGER_PAGE_SIZE / ROW_SIZE;
// Temporary limit, later the limit will be the maximum file size supported by
// the OS. We will still need to limit the amount of pages in memory.
static const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * PAGER_MAX_PAGES;

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
  uint32_t num_rows;
} Table;

// Opens a connection to a database.
Table *table_db_open(const char *filename);

// Closes a connection to a database.
void table_db_close(Table *table);

// Handle memory I/O for a particular row.
void *table_row_slot(Table *table, uint32_t row_num);

#endif
