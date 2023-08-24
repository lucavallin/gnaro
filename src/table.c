#include "table.h"
#include "log.h"
#include "node.h"
#include "pager.h"
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

// Opens a connection to the database by opening the database file and
// initializing a pager and a table data structure
Table *table_db_open(const char *filename) {
  Pager *pager = pager_open(filename);

  log_debug("allocating table...");
  Table *table = malloc(sizeof(Table));
  table->pager = pager;
  table->root_page_num = 0;

  if (pager->num_pages == 0) {
    log_debug("database file is empty, initializing new database...");
    void *root_node = pager_get_page(pager, 0);
    node_leaf_initialize(root_node);
  }

  return table;
}

// table_db_close flushes the page cache to disk, closes the database file and
// then frees the memory for the Pager and Table data structures
void table_db_close(Table *table) {
  log_debug("closing database...");

  Pager *pager = table->pager;

  log_debug("flushing %d pages...", pager->num_pages);
  for (uint32_t i = 0; i < pager->num_pages; i++) {
    if (pager->pages[i] == NULL) {
      log_debug("page %d is empty, skipping...", i);
      continue;
    }

    log_debug("flushing page %d...", i);
    pager_flush(pager, i);

    log_debug("freeing page %d...", i);
    free(pager->pages[i]);

    log_debug("setting page %d to NULL...", i);
    pager->pages[i] = NULL;
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
