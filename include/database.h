#ifndef DATABASE_H
#define DATABASE_H

#include "pager.h"
#include <stdint.h>

// DatabaseResult is an enum that represents the result of a database operation.
typedef enum {
  DATABASE_CLOSE_SUCCESS,
  DATABASE_CLOSE_FAIL,
} DatabaseResult;

// Database is a struct that represents a database. It uses a pager to persist
// data. It roughly corresponds to a B-Tree in SQLite.
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
} Database;

// Opens a connection to a database.
Database *database_open(const char *filename);

// Closes a connection to a database.
DatabaseResult database_close(Database *database);

#endif
