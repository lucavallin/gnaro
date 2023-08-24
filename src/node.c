#include "node.h"
#include "log.h"
#include <stdint.h>

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
