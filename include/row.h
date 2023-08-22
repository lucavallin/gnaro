#ifndef ROW_H
#define ROW_H

#include <stdint.h>

// Hardcoded Row for now.
enum { COLUMN_USERNAME_SIZE = 32, COLUMN_EMAIL_SIZE = 255 };
typedef struct {
  uint32_t id;
  char username[COLUMN_USERNAME_SIZE];
  char email[COLUMN_EMAIL_SIZE];
} Row;

// Compact representation of a row, hardcoded for now
#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)
static const uint32_t ID_SIZE = size_of_attribute(Row, id);
static const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
static const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
static const uint32_t ID_OFFSET = 0;
static const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
static const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
static const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

// Serialize a row for storage in a table.
void row_serialize(Row *source, void *destination);

// Deserialize a row from storage in a table.
void row_deserialize(void *source, Row *destination);

// Print a row to stdout.
void row_print(Row *row);

#endif
