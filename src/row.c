#include "../include/row.h"
#include "../lib/log/log.h"
#include <stdio.h>
#include <string.h>

// strncpy initializes all bytes even if the string is shorter than the size
void row_serialize(Row *source, void *destination) {
  log_debug("serializing row...");
  memcpy(destination + ROW_ID_OFFSET, &(source->id), ROW_ID_SIZE);
  strncpy(destination + ROW_USERNAME_OFFSET, source->username,
          ROW_USERNAME_SIZE);
  strncpy(destination + ROW_EMAIL_OFFSET, source->email, ROW_EMAIL_SIZE);
}

void row_deserialize(void *source, Row *destination) {
  log_debug("deserializing row...");
  memcpy(&(destination->id), source + ROW_ID_OFFSET, ROW_ID_SIZE);
  memcpy(&(destination->username), source + ROW_USERNAME_OFFSET,
         ROW_USERNAME_SIZE);
  memcpy(&(destination->email), source + ROW_EMAIL_OFFSET, ROW_EMAIL_SIZE);
}

void row_print(Row *row) {
  printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}
