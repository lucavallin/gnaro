#include "node.h"
#include "log.h"
#include "pager.h"
#include "row.h"
#include <cursor.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t *node_leaf_num_cells(void *node) {
  log_debug("getting num_cells from node...");
  return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

void *node_leaf_cell(void *node, uint32_t cell_num) {
  log_debug("getting cell %d from node...", cell_num);
  return node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

uint32_t *node_leaf_key(void *node, uint32_t cell_num) {
  log_debug("getting key from cell %d...", cell_num);
  return node_leaf_cell(node, cell_num);
}

void *node_leaf_value(void *node, uint32_t cell_num) {
  log_debug("getting value from cell %d...", cell_num);
  return node_leaf_cell(node, cell_num) + LEAF_NODE_KEY_SIZE;
}

void node_leaf_initialize(void *node) {
  log_debug("initializing node...");
  *node_leaf_num_cells(node) = 0;
}

void node_leaf_insert(Cursor *cursor, uint32_t key, Row *value) {
  log_debug("inserting row into node...");
  void *node = pager_get_page(cursor->table->pager, cursor->page_num);

  uint32_t num_cells = *node_leaf_num_cells(node);
  if (num_cells >= LEAF_NODE_MAX_CELLS) {
    log_error("node full: splitting not yet implemented");
    exit(EXIT_FAILURE);
  }

  if (cursor->cell_num < num_cells) {
    log_debug("making room for new cell...");
    for (uint32_t i = num_cells; i > cursor->cell_num; i--) {
      log_debug("moving cell %d to the right...", i);
      memcpy(node_leaf_cell(node, i), node_leaf_cell(node, i - 1),
             LEAF_NODE_CELL_SIZE);
    }
  }

  log_debug("incrementing num_cells...");
  *(node_leaf_num_cells(node)) += 1;

  log_debug("setting key...");
  *(node_leaf_key(node, cursor->cell_num)) = key;

  log_debug("serializing row...");
  row_serialize(value, node_leaf_value(node, cursor->cell_num));

  log_debug("row inserted into node");
}

void node_leaf_print(void *node) {
  uint32_t num_cells = *node_leaf_num_cells(node);
  printf("leaf (size %d)\n", num_cells);
  for (uint32_t i = 0; i < num_cells; i++) {
    uint32_t key = *node_leaf_key(node, i);
    printf("  - %d : %d\n", i, key);
  }
}
