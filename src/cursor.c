#include "cursor.h"
#include "log.h"
#include "node.h"
#include "pager.h"
#include "table.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// Get cell 0 of the leftmost leaf node
Cursor *cursor_at_start(Table *table) {
  log_debug("allocating cursor at start of table...");
  Cursor *cursor = cursor_find_key(table, 0);

  log_debug("getting root node...");
  void *node = pager_get_page(table->pager, cursor->page_num);
  uint32_t num_cells = *node_leaf_num_cells(node);
  cursor->end_of_table = (num_cells == 0);

  return cursor;
}

// Search the tree for the given key.
Cursor *cursor_find_key(Table *table, uint32_t key) {
  log_debug("finding key %d...", key);
  uint32_t root_page_num = table->root_page_num;
  void *root_node = pager_get_page(table->pager, root_page_num);

  if (node_get_type(root_node) == NODE_TYPE_LEAF) {
    log_debug("searching leaf node...");
    return node_leaf_find(table, root_page_num, key);
  }

  log_debug("searching internal node...");
  return node_internal_find(table, root_page_num, key);
}

void cursor_advance(Cursor *cursor) {
  log_debug("advancing cursor to page %d...", cursor->page_num);
  uint32_t page_num = cursor->page_num;
  void *node = pager_get_page(cursor->table->pager, page_num);

  cursor->cell_num += 1;
  if (cursor->cell_num >= (*node_leaf_num_cells(node))) {
    log_debug("advancing to next page...");
    uint32_t next_page_num = *node_leaf_next(node);
    if (next_page_num == 0) {
      log_debug("cursor is at end of table...");
      cursor->end_of_table = true;
    } else {
      log_debug("cursor is not at end of table...");
      cursor->page_num = next_page_num;
      cursor->cell_num = 0;
    }
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
