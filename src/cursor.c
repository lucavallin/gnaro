#include "cursor.h"
#include "log.h"
#include "node.h"
#include "pager.h"
#include "table.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

Cursor *cursor_at_start(Table *table) {
  log_debug("allocating cursor at start of table...");
  Cursor *cursor = malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->page_num = table->root_page_num;
  cursor->cell_num = 0;

  log_debug("getting root node...");
  void *root_node = pager_get_page(table->pager, table->root_page_num);
  uint32_t num_cells = *node_leaf_num_cells(root_node);
  cursor->end_of_table = (num_cells == 0);

  return cursor;
}

Cursor *cursor_at_end(Table *table) {
  log_debug("allocating cursor at end of table...");
  Cursor *cursor = malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->page_num = table->root_page_num;

  log_debug("getting root node...");
  void *root_node = pager_get_page(table->pager, table->root_page_num);
  uint32_t num_cells = *node_leaf_num_cells(root_node);
  cursor->cell_num = num_cells;
  cursor->end_of_table = true;

  return cursor;
}

void cursor_advance(Cursor *cursor) {
  log_debug("advancing cursor to page %d...", cursor->page_num);
  uint32_t page_num = cursor->page_num;
  void *node = pager_get_page(cursor->table->pager, page_num);

  cursor->cell_num += 1;
  if (cursor->cell_num >= (*node_leaf_num_cells(node))) {
    log_debug("cursor is at end of table...");
    cursor->end_of_table = true;
  }
}

// The table is a tree, therefore we identify a position by the page number of
// the node, and the cell number within that node.
void *cursor_value(Cursor *cursor) {
  log_debug("getting cursor value...");
  uint32_t page_num = cursor->page_num;
  void *page = pager_get_page(cursor->table->pager, page_num);

  log_debug("getting node value from page %d...", page_num);

  return node_leaf_value(page, cursor->cell_num);
}

void cursor_close(Cursor *cursor) {
  log_debug("freeing cursor...");
  free(cursor);
  log_debug("cursor freed");
}
