#ifndef ROW_H
#define ROW_H

#include <stdint.h>

// Hardcoded Row for now.
enum { ROW_COLUMN_USERNAME_SIZE = 32, ROW_COLUMN_EMAIL_SIZE = 255 };
typedef struct {
  uint32_t id;
  char username[ROW_COLUMN_USERNAME_SIZE + 1];
  char email[ROW_COLUMN_EMAIL_SIZE + 1];
} Row;

// Compact representation of a row, hardcoded for now
#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)
static const uint32_t ROW_ID_SIZE = size_of_attribute(Row, id);
static const uint32_t ROW_USERNAME_SIZE = size_of_attribute(Row, username);
static const uint32_t ROW_EMAIL_SIZE = size_of_attribute(Row, email);
static const uint32_t ROW_ID_OFFSET = 0;
static const uint32_t ROW_USERNAME_OFFSET = ROW_ID_OFFSET + ROW_ID_SIZE;
static const uint32_t ROW_EMAIL_OFFSET =
    ROW_USERNAME_OFFSET + ROW_USERNAME_SIZE;
static const uint32_t ROW_SIZE =
    ROW_ID_SIZE + ROW_USERNAME_SIZE + ROW_EMAIL_SIZE;

// Serialize a row for storage in a table.
void row_serialize(Row *source, void *destination);

// Deserialize a row from storage in a table.
void row_deserialize(void *source, Row *destination);

// Print a row to stdout.
void row_print(Row *row);

#endif
