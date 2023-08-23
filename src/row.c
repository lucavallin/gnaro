#include "row.h"
#include "log.h"
#include <stdio.h>
#include <string.h>

void row_serialize(Row *source, void *destination) {
  log_debug("serializing row...");
  memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
  // strncpy ensures that all bytes are initialized, even if the string is not
  // as long as the field
  strncpy(destination + USERNAME_OFFSET, source->username, USERNAME_SIZE);
  strncpy(destination + EMAIL_OFFSET, source->email, EMAIL_SIZE);
}

void row_deserialize(void *source, Row *destination) {
  log_debug("deserializing row...");
  memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
  memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
  memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

void row_print(Row *row) {
  printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}
