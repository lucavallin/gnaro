#include "cursor.h"
#include "btree.h"
#include "database.h"
#include "log.h"
#include "pager.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// Get cell 0 of the leftmost leaf node
Cursor *cursor_start(Database *database) {
  log_debug("allocating cursor at start of database...");
  Cursor *cursor = cursor_find_key(database, 0);

  log_debug("getting root node...");
  void *node = pager_get_page(database->pager, cursor->page_num);
  uint32_t num_cells = *btree_node_leaf_num_cells(node);
  cursor->end_of_table = (num_cells == 0);

  return cursor;
}

// Search the tree for the given key.
Cursor *cursor_find_key(Database *database, uint32_t key) {
  log_debug("finding key %d...", key);
  uint32_t root_page_num = database->root_page_num;
  void *root_node = pager_get_page(database->pager, root_page_num);

  if (btree_node_get_type(root_node) == BTREE_NODE_TYPE_LEAF) {
    log_debug("searching leaf node...");
    return btree_node_leaf_find(database, key, root_page_num);
  }

  log_debug("searching internal node...");
  return btree_node_internal_find(database, key, root_page_num);
}

void cursor_advance(Cursor *cursor) {
  log_debug("advancing cursor to page %d...", cursor->page_num);
  uint32_t page_num = cursor->page_num;
  void *node = pager_get_page(cursor->database->pager, page_num);

  cursor->cell_num += 1;
  if (cursor->cell_num >= (*btree_node_leaf_num_cells(node))) {
    log_debug("advancing to next page...");
    uint32_t next_page_num = *btree_node_leaf_next(node);
    if (next_page_num == 0) {
      log_debug("cursor is at end of database...");
      cursor->end_of_table = true;
    } else {
      log_debug("cursor is not at end of database...");
      cursor->page_num = next_page_num;
      cursor->cell_num = 0;
    }
  }
}

// The database is a tree, therefore we identify a position by the page number
// of the node, and the cell number within that node.
void *cursor_value(Cursor *cursor) {
  log_debug("getting cursor value...");
  uint32_t page_num = cursor->page_num;
  void *page = pager_get_page(cursor->database->pager, page_num);

  log_debug("getting node value from page %d...", page_num);

  return btree_node_leaf_value(page, cursor->cell_num);
}

void cursor_close(Cursor *cursor) {
  log_debug("freeing cursor...");
  free(cursor);
  log_debug("cursor freed");
}
