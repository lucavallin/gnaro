#include "cursor.h"
#include "log.h"
#include "pager.h"
#include "row.h"
#include "table.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

Cursor *cursor_at_start(Table *table) {
  log_debug("allocating cursor at start of table...");
  Cursor *cursor = malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->row_num = 0;
  cursor->end_of_table = (table->num_rows == 0);

  return cursor;
}

Cursor *cursor_at_end(Table *table) {
  log_debug("allocating cursor at end of table...");
  Cursor *cursor = malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->row_num = table->num_rows;
  cursor->end_of_table = true;

  return cursor;
}

void cursor_advance(Cursor *cursor) {
  cursor->row_num += 1;
  log_debug("advancing cursor to row %d...", cursor->row_num);

  if (cursor->row_num >= cursor->table->num_rows) {
    log_debug("cursor is at end of table...");
    cursor->end_of_table = true;
  }
}

void *cursor_value(Cursor *cursor) {
  log_debug("getting cursor value...");
  uint32_t row_num = cursor->row_num;
  uint32_t page_num = row_num / ROWS_PER_PAGE;
  void *page = pager_get_page(cursor->table->pager, page_num);

  uint32_t row_offset = row_num % ROWS_PER_PAGE;
  uint32_t byte_offset = row_offset * ROW_SIZE;

  log_debug("cursor value is at page %d, row %d, byte %d...", page_num,
            row_offset, byte_offset);

  return page + byte_offset;
}

void cursor_close(Cursor *cursor) {
  log_debug("freeing cursor...");
  free(cursor);
  log_debug("cursor freed");
}
