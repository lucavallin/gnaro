#ifndef TABLE_H
#define TABLE_H

#include "row.h"
#include <stdint.h>

// Pages and rows configuration
enum { TABLE_MAX_PAGES = 100 };
// 4 kilobytes, same size as a virtual memory page in most architectures,
// so that a table page corresponds to a single memory page for the OS.
static const uint32_t PAGE_SIZE = 4096;
static const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
// Temporary limit, later the limit will be the maximum file size supported by
// the OS. We will still need to limit the amount of pages in memory.
static const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

// Table is a struct that represents a table. SQLite uses a B-tree for fast
// lookups, inserts and deletes. We’ll start with something simpler. Like a
// B-tree, it will group rows into pages, but instead of arranging those pages
// as a tree it will arrange them as an array.
//
// Data will be stored as follows:
// - Store rows in blocks of memory called pages
// - Each page stores as many rows as it can fit
// - Rows are serialized into a compact representation with each page
// - Pages are only allocated as needed
// - Keep a fixed-size array of pointers to pages
typedef struct {
  uint32_t num_rows;
  void *pages[TABLE_MAX_PAGES];
} Table;

// Handle memory I/O for a particular row.
void *table_row_slot(Table *table, uint32_t row_num);

// Create a new table.
Table *table_new(void);

// Free a table.
void table_free(Table *table);

#endif
