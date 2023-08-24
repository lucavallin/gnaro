#include "table.h"
#include "log.h"
#include "pager.h"
#include "row.h"
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

// Opens a connection to the database by opening the database file and
// initializing a pager and a table data structure
Table *table_db_open(const char *filename) {
  Pager *pager = pager_open(filename);
  uint32_t num_rows = pager->file_length / ROW_SIZE;

  log_debug("allocating table...");
  Table *table = malloc(sizeof(Table));
  table->pager = pager;
  table->num_rows = num_rows;

  return table;
}

// table_db_close flushes the page cache to disk, closes the database file and
// then frees the memory for the Pager and Table data structures
void table_db_close(Table *table) {
  log_debug("closing database...");

  Pager *pager = table->pager;
  uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

  log_debug("flushing %d full pages...", num_full_pages);
  for (uint32_t i = 0; i < num_full_pages; i++) {
    if (pager->pages[i] == NULL) {
      log_debug("page %d is empty, skipping...", i);
      continue;
    }

    log_debug("flushing page %d...", i);
    pager_flush(pager, i, PAGER_PAGE_SIZE);

    log_debug("freeing page %d...", i);
    free(pager->pages[i]);

    log_debug("setting page %d to NULL...", i);
    pager->pages[i] = NULL;
  }

  // There may be a partial page to write to the end of the file
  // This should not be needed after we switch to a B-tree
  log_debug("flushing partial page(s)...");

  uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
  if (num_additional_rows > 0) {
    uint32_t page_num = num_full_pages;
    if (pager->pages[page_num] != NULL) {
      log_debug("flushing page %d...", page_num);

      pager_flush(pager, page_num, num_additional_rows * ROW_SIZE);

      log_debug("freeing page %d...", page_num);
      free(pager->pages[page_num]);

      log_debug("setting page %d to NULL...", page_num);
      pager->pages[page_num] = NULL;
    }
  }

  log_debug("closing file descriptor...");
  int result = close(pager->file_descriptor);
  if (result == -1) {
    log_error("failed to close db file: %m");
    exit(EXIT_FAILURE);
  }

  log_debug("freeing pages...");
  for (uint32_t i = 0; i < PAGER_MAX_PAGES; i++) {
    void *page = pager->pages[i];
    if (page) {
      log_debug("freeing page %d...", i);
      free(page);

      log_debug("setting page %d to NULL...", i);
      pager->pages[i] = NULL;
    }
  }

  log_debug("freeing pager...");
  free(pager);

  log_debug("freeing table...");
  free(table);
}
