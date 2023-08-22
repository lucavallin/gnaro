#include "table.h"
#include "log.h"
#include "row.h"
#include <stdint.h>
#include <stdlib.h>

void *table_row_slot(Table *table, uint32_t row_num) {
  uint32_t page_num = row_num / ROWS_PER_PAGE;

  void *page = table->pages[page_num];
  if (page == NULL) {
    // Allocate memory only when we try to access page
    log_debug("page %d not found...", page_num);
    log_debug("allocating memory for page %d...", page_num);
    page = table->pages[page_num] = malloc(PAGE_SIZE);
  }

  log_debug("page %d found...", page_num);

  uint32_t row_offset = row_num % ROWS_PER_PAGE;
  uint32_t byte_offset = row_offset * ROW_SIZE;
  return page + byte_offset;
}

Table *table_new() {
  Table *table = (Table *)malloc(sizeof(Table));
  table->num_rows = 0;
  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
    table->pages[i] = NULL;
  }
  return table;
}

void table_free(Table *table) {
  for (int i = 0; table->pages[i]; i++) {
    free(table->pages[i]);
  }
  free(table);
}
