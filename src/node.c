#include "node.h"
#include "log.h"
#include "pager.h"
#include "row.h"
#include <cursor.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t *node_leaf_num_cells(void *node) {
  log_debug("getting num_cells from node...");
  return node + NODE_LEAF_NUM_CELLS_OFFSET;
}

void *node_leaf_cell(void *node, uint32_t cell_num) {
  log_debug("getting cell %d from node...", cell_num);
  return node + NODE_LEAF_HEADER_SIZE +
         (size_t)(cell_num * NODE_LEAF_CELL_SIZE);
}

uint32_t *node_leaf_key(void *node, uint32_t cell_num) {
  log_debug("getting key from cell %d...", cell_num);
  return node_leaf_cell(node, cell_num);
}

void *node_leaf_value(void *node, uint32_t cell_num) {
  log_debug("getting value from cell %d...", cell_num);
  return node_leaf_cell(node, cell_num) + NODE_LEAF_KEY_SIZE;
}

void node_leaf_initialize(void *node) {
  log_debug("initializing leaf node...");
  node_set_type(node, NODE_TYPE_LEAF);
  node_set_root(node, false);
  *node_leaf_num_cells(node) = 0;
  // Set the next leaf to 0 (no sibling)
  *node_leaf_next(node) = 0;
}

void node_leaf_insert(Cursor *cursor, uint32_t key, Row *value) {
  log_debug("inserting row into node...");
  void *node = pager_get_page(cursor->table->pager, cursor->page_num);

  uint32_t num_cells = *node_leaf_num_cells(node);
  if (num_cells >= NODE_LEAF_MAX_CELLS) {
    node_leaf_split_and_insert(cursor, key, value);
    return;
  }

  if (cursor->cell_num < num_cells) {
    log_debug("making room for new cell...");
    for (uint32_t i = num_cells; i > cursor->cell_num; i--) {
      log_debug("moving cell %d to the right...", i);
      memcpy(node_leaf_cell(node, i), node_leaf_cell(node, i - 1),
             NODE_LEAF_CELL_SIZE);
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

// Returns the position of the key, or the position of another key to move to
// for inserting the new key, or the position one past the last key
Cursor *node_leaf_find(Table *table, uint32_t page_num, uint32_t key) {
  log_debug("finding key %d in node...", key);
  void *node = pager_get_page(table->pager, page_num);
  uint32_t num_cells = *node_leaf_num_cells(node);

  log_debug("allocating cursor...");
  Cursor *cursor = malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->page_num = page_num;

  log_debug("binary searching for key %d...", key);
  uint32_t min_index = 0;
  uint32_t one_past_max_index = num_cells;
  while (one_past_max_index != min_index) {
    log_debug("min_index: %d, one_past_max_index: %d...", min_index,
              one_past_max_index);
    uint32_t index = (min_index + one_past_max_index) / 2;
    uint32_t key_at_index = *node_leaf_key(node, index);

    if (key == key_at_index) {
      log_debug("key found at index %d", index);
      cursor->cell_num = index;
      return cursor;
    }
    if (key < key_at_index) {
      log_debug("key is less than key at index %d...", index);
      one_past_max_index = index;
    } else {
      log_debug("key is greater than key at index %d...", index);
      min_index = index + 1;
    }
  }

  log_debug("setting cursor at index %d...", min_index);
  cursor->cell_num = min_index;
  return cursor;
}

NodeType node_get_type(void *node) {
  log_debug("getting node type...");
  // Cast to uint8_t to ensure it is serialized as a single byte
  uint8_t value = *((uint8_t *)(node + NODE_TYPE_OFFSET));
  return (NodeType)value;
}

void node_set_type(void *node, NodeType type) {
  log_debug("setting node type to %d...", type);
  uint8_t value = type;
  *((uint8_t *)(node + NODE_TYPE_OFFSET)) = value;
}

void node_leaf_split_and_insert(Cursor *cursor, uint32_t key, Row *value) {
  // Create a new node and move half the cells over.
  // Insert the new value in one of the two nodes.
  // Update parent or create a new parent.
  log_debug("splitting node and inserting row...");
  void *old_node = pager_get_page(cursor->table->pager, cursor->page_num);
  uint32_t old_max = node_get_max_key(cursor->table->pager, old_node);
  uint32_t new_page_num = pager_get_unused_page_num(cursor->table->pager);
  void *new_node = pager_get_page(cursor->table->pager, new_page_num);

  log_debug("initializing new node...");
  node_leaf_initialize(new_node);
  *node_parent(new_node) = *node_parent(old_node);
  *node_leaf_next(new_node) = *node_leaf_next(old_node);
  *node_leaf_next(old_node) = new_page_num;

  log_debug("dividing keys evenly between old (left) and new (right) nodes...");
  for (int32_t i = (int32_t)NODE_LEAF_MAX_CELLS; i >= 0; i--) {
    log_debug("moving cell %d...", i);

    void *destination_node;
    if (i >= NODE_LEAF_LEFT_SPLIT_COUNT) {
      destination_node = new_node;
    } else {
      destination_node = old_node;
    }
    uint32_t index_within_node = i % NODE_LEAF_LEFT_SPLIT_COUNT;
    void *destination = node_leaf_cell(destination_node, index_within_node);

    if (i == cursor->cell_num) {
      log_debug("inserting row into new node...");
      row_serialize(value,
                    node_leaf_value(destination_node, index_within_node));
      *node_leaf_key(destination_node, index_within_node) = key;
    } else if (i > cursor->cell_num) {
      memcpy(destination, node_leaf_cell(old_node, i - 1), NODE_LEAF_CELL_SIZE);
    } else {
      memcpy(destination, node_leaf_cell(old_node, i), NODE_LEAF_CELL_SIZE);
    }
  }

  log_debug("updating cell counts...");
  *(node_leaf_num_cells(old_node)) = NODE_LEAF_LEFT_SPLIT_COUNT;
  *(node_leaf_num_cells(new_node)) = NODE_LEAF_RIGHT_SPLIT_COUNT;

  log_debug("updating parent node...");
  if (node_is_root(old_node)) {
    return node_create_new_root(cursor->table, new_page_num);
  }

  uint32_t parent_page_num = *node_parent(old_node);
  uint32_t new_max = node_get_max_key(cursor->table->pager, old_node);
  void *parent = pager_get_page(cursor->table->pager, parent_page_num);

  node_internal_update_key(parent, old_max, new_max);
  node_internal_insert(cursor->table, parent_page_num, new_page_num);
}

uint32_t *node_leaf_next(void *node) {
  return node + NODE_LEAF_NEXT_LEAF_OFFSET;
}

void node_create_new_root(Table *table, uint32_t right_child_page_num) {
  // Handle splitting the root.
  // Old root copied to new page, becomes left child.
  // Address of right child passed in.
  // Re-initialize root page to contain the new root node.
  // New root node points to two children.
  log_debug("creating new root node...");
  void *root = pager_get_page(table->pager, table->root_page_num);
  void *right_child = pager_get_page(table->pager, right_child_page_num);
  uint32_t left_child_page_num = pager_get_unused_page_num(table->pager);
  void *left_child = pager_get_page(table->pager, left_child_page_num);

  if (node_get_type(root) == NODE_TYPE_INTERNAL) {
    node_internal_initialize(right_child);
    node_internal_initialize(left_child);
  }

  log_debug("copying old root to left child...");
  memcpy(left_child, root, PAGER_PAGE_SIZE);
  node_set_root(left_child, false);

  if (node_get_type(left_child) == NODE_TYPE_INTERNAL) {
    void *child;
    for (int i = 0; i < *node_internal_num_keys(left_child); i++) {
      child = pager_get_page(table->pager, *node_internal_child(left_child, i));
      *node_parent(child) = left_child_page_num;
    }
    child =
        pager_get_page(table->pager, *node_internal_right_child(left_child));
    *node_parent(child) = left_child_page_num;
  }

  // Root node is a new internal node with one key and two children
  log_debug("initializing new root node...");
  node_internal_initialize(root);
  node_set_root(root, true);
  *node_internal_num_keys(root) = 1;
  *node_internal_child(root, 0) = left_child_page_num;
  uint32_t left_child_max_key = node_get_max_key(table->pager, left_child);
  *node_internal_key(root, 0) = left_child_max_key;
  *node_internal_right_child(root) = right_child_page_num;
  *node_parent(left_child) = table->root_page_num;
  *node_parent(right_child) = table->root_page_num;
}

uint32_t *node_internal_num_keys(void *node) {
  log_debug("getting num_keys from node...");
  return node + NODE_INTERNAL_NUM_KEYS_OFFSET;
}

uint32_t *node_internal_right_child(void *node) {
  log_debug("getting right_child from node...");
  return node + NODE_INTERNAL_RIGHT_CHILD_OFFSET;
}

uint32_t *node_internal_cell(void *node, uint32_t cell_num) {
  log_debug("getting cell %d from node...", cell_num);
  return node + NODE_INTERNAL_HEADER_SIZE +
         (size_t)(cell_num * NODE_INTERNAL_CELL_SIZE);
}

uint32_t *node_internal_child(void *node, uint32_t child_num) {
  log_debug("getting child %d from node...", child_num);
  uint32_t num_keys = *node_internal_num_keys(node);

  if (child_num > num_keys) {
    log_error("failed to access child_num %d, max num_keys is %d\n", child_num,
              num_keys);
    exit(EXIT_FAILURE);
  } else if (child_num == num_keys) {
    log_debug("getting right_child from node...");
    uint32_t *right_child = node_internal_right_child(node);
    if (*right_child == NODE_INTERNAL_INVALID_PAGE_NUM) {
      log_error("right child of node is an invalid page");
      exit(EXIT_FAILURE);
    }
    return right_child;
  } else {
    log_debug("getting child %d from node...", child_num);
    uint32_t *child = node_internal_cell(node, child_num);
    if (*child == NODE_INTERNAL_INVALID_PAGE_NUM) {
      log_error("right child %d of node is an invalid page", child_num);
      exit(EXIT_FAILURE);
    }
    return child;
  }
}

uint32_t *node_internal_key(void *node, uint32_t key_num) {
  log_debug("getting key %d from node...", key_num);
  return (void *)node_internal_cell(node, key_num) + NODE_INTERNAL_CHILD_SIZE;
}

uint32_t node_get_max_key(Pager *pager, void *node) {
  if (node_get_type(node) == NODE_TYPE_LEAF) {
    return *node_leaf_key(node, *node_leaf_num_cells(node) - 1);
  }
  void *right_child = pager_get_page(pager, *node_internal_right_child(node));
  return node_get_max_key(pager, right_child);
}

bool node_is_root(void *node) {
  log_debug("checking if node is root...");
  uint8_t value = *((uint8_t *)(node + NODE_IS_ROOT_OFFSET));
  return (bool)value;
}

void node_set_root(void *node, bool is_root) {
  log_debug("setting node root status to %d...", is_root);
  uint8_t value = is_root;
  *((uint8_t *)(node + NODE_IS_ROOT_OFFSET)) = value;
}

void node_internal_initialize(void *node) {
  log_debug("initializing internal node...");
  node_set_type(node, NODE_TYPE_INTERNAL);
  node_set_root(node, false);
  *node_internal_num_keys(node) = 0;
  log_debug("setting right child to invalid page number to avoid root parent "
            "bug...");
  *node_internal_right_child(node) = NODE_INTERNAL_INVALID_PAGE_NUM;
}

Cursor *node_internal_find(Table *table, uint32_t page_num, uint32_t key) {
  log_debug("finding key %d in internal node...", key);
  void *node = pager_get_page(table->pager, page_num);
  uint32_t child_index = node_internal_find_child(node, key);
  uint32_t child_num = *node_internal_child(node, child_index);
  void *child = pager_get_page(table->pager, child_num);

  switch (node_get_type(child)) {
  case NODE_TYPE_LEAF:
    return node_leaf_find(table, child_num, key);
  case NODE_TYPE_INTERNAL:
    return node_internal_find(table, child_num, key);
  }
}

uint32_t node_internal_find_child(void *node, uint32_t key) {
  log_debug("finding child for key %d...", key);
  uint32_t num_keys = *node_internal_num_keys(node);

  log_debug("binary searching for key %d...", key);
  uint32_t min_index = 0;
  uint32_t max_index = num_keys;

  while (min_index != max_index) {
    log_debug("min_index: %d, max_index: %d...", min_index, max_index);
    uint32_t index = (min_index + max_index) / 2;
    uint32_t key_to_right = *node_internal_key(node, index);
    if (key_to_right >= key) {
      log_debug("key is less than key at index %d...", index);
      max_index = index;
    } else {
      log_debug("key is greater than key at index %d...", index);
      min_index = index + 1;
    }
  }

  log_debug("returning min_index %d...", min_index);
  return min_index;
}

uint32_t *node_parent(void *node) {
  log_debug("getting parent from node...");
  return node + NODE_PARENT_POINTER_OFFSET;
}

void node_internal_update_key(void *node, uint32_t old_key, uint32_t new_key) {
  uint32_t old_child_index = node_internal_find_child(node, old_key);
  *node_internal_key(node, old_child_index) = new_key;
}

void node_internal_insert(Table *table, uint32_t parent_page_num,
                          uint32_t child_page_num) {
  log_debug("inserting new child into internal node...");
  void *parent = pager_get_page(table->pager, parent_page_num);
  void *child = pager_get_page(table->pager, child_page_num);
  uint32_t child_max_key = node_get_max_key(table->pager, child);
  uint32_t index = node_internal_find_child(parent, child_max_key);

  uint32_t original_num_keys = *node_internal_num_keys(parent);
  *node_internal_num_keys(parent) = original_num_keys + 1;

  if (original_num_keys >= NODE_INTERNAL_MAX_CELLS) {
    node_internal_split_and_insert(table, parent_page_num, child_page_num);
    return;
  }

  uint32_t right_child_page_num = *node_internal_right_child(parent);

  if (right_child_page_num == NODE_INTERNAL_INVALID_PAGE_NUM) {
    log_debug("node is empty...");
    *node_internal_right_child(parent) = child_page_num;
    return;
  }

  void *right_child = pager_get_page(table->pager, right_child_page_num);

  log_debug("incrementing num_keys...");
  *node_internal_num_keys(parent) = original_num_keys + 1;

  if (child_max_key > node_get_max_key(table->pager, right_child)) {
    log_debug("replace right child...");
    *node_internal_child(parent, original_num_keys) = right_child_page_num;
    *node_internal_key(parent, original_num_keys) =
        node_get_max_key(table->pager, right_child);
    *node_internal_right_child(parent) = child_page_num;
  } else {
    log_debug("making room for new cell...");
    for (uint32_t i = original_num_keys; i > index; i--) {
      void *destination = node_internal_cell(parent, i);
      void *source = node_internal_cell(parent, i - 1);
      memcpy(destination, source, NODE_INTERNAL_CELL_SIZE);
    }
    *node_internal_child(parent, index) = child_page_num;
    *node_internal_key(parent, index) = child_max_key;
  }
}

void node_internal_split_and_insert(Table *table, uint32_t parent_page_num,
                                    uint32_t child_page_num) {
  log_debug("splitting internal node and inserting new child...");
  uint32_t old_page_num = parent_page_num;
  void *old_node = pager_get_page(table->pager, parent_page_num);
  uint32_t old_max = node_get_max_key(table->pager, old_node);

  void *child = pager_get_page(table->pager, child_page_num);
  uint32_t child_max = node_get_max_key(table->pager, child);

  uint32_t new_page_num = pager_get_unused_page_num(table->pager);

  log_debug("checking if node is root...");
  uint32_t splitting_root = node_is_root(old_node);

  void *parent;
  void *new_node;
  if (splitting_root) {
    log_debug("splitting root node...");
    node_create_new_root(table, new_page_num);
    parent = pager_get_page(table->pager, table->root_page_num);

    log_debug("updating old node to point to new root's left child...");
    old_page_num = *node_internal_child(parent, 0);
    old_node = pager_get_page(table->pager, old_page_num);
  } else {
    log_debug("splitting non-root node...");
    parent = pager_get_page(table->pager, *node_parent(old_node));
    new_node = pager_get_page(table->pager, new_page_num);
    node_internal_initialize(new_node);
  }

  uint32_t *old_num_keys = node_internal_num_keys(old_node);

  uint32_t cur_page_num = *node_internal_right_child(old_node);
  void *cur = pager_get_page(table->pager, cur_page_num);

  log_debug("moving right child to new node...");
  node_internal_insert(table, new_page_num, cur_page_num);
  *node_parent(cur) = new_page_num;
  *node_internal_right_child(old_node) = NODE_INTERNAL_INVALID_PAGE_NUM;

  log_debug("moving keys and children to new node...");
  for (uint32_t i = NODE_INTERNAL_MAX_CELLS - 1;
       i > NODE_INTERNAL_MAX_CELLS / 2; i--) {
    log_debug("moving cell %d...", i);
    cur_page_num = *node_internal_child(old_node, i);
    cur = pager_get_page(table->pager, cur_page_num);

    node_internal_insert(table, new_page_num, cur_page_num);
    *node_parent(cur) = new_page_num;

    (*old_num_keys)--;
  }

  log_debug("updating old node to point to new node's left child...");
  *node_internal_right_child(old_node) =
      *node_internal_child(old_node, *old_num_keys - 1);
  (*old_num_keys)--;

  log_debug(
      "determining which node should contain the child to be inserted...");
  uint32_t max_after_split = node_get_max_key(table->pager, old_node);

  uint32_t destination_page_num =
      child_max < max_after_split ? old_page_num : new_page_num;

  log_debug("inserting new child into destination node...");
  node_internal_split_and_insert(table, destination_page_num, child_page_num);
  *node_parent(child) = destination_page_num;

  log_debug("updating parent node...");
  node_internal_update_key(parent, old_max,
                           node_get_max_key(table->pager, old_node));

  if (!splitting_root) {
    log_debug("not a root node, updating parent...");
    node_internal_insert(table, *node_parent(old_node), new_page_num);
    *node_parent(new_node) = *node_parent(old_node);
  }
}

void node_print_tree(Pager *pager, uint32_t page_num,
                     uint32_t indentation_level) {
  void *node = pager_get_page(pager, page_num);
  uint32_t num_keys;
  uint32_t child;

  switch (node_get_type(node)) {
  case (NODE_TYPE_LEAF):
    num_keys = *node_leaf_num_cells(node);
    for (uint32_t i = 0; i < indentation_level; i++) {
      printf("  ");
    }
    printf("- leaf (size %d)\n", num_keys);
    for (uint32_t i = 0; i < num_keys; i++) {
      for (uint32_t i = 0; i < indentation_level + 1; i++) {
        printf("  ");
      }
      printf("- %d\n", *node_leaf_key(node, i));
    }
    break;
  case (NODE_TYPE_INTERNAL):
    num_keys = *node_internal_num_keys(node);
    for (uint32_t i = 0; i < indentation_level; i++) {
      printf("  ");
    }
    printf("- internal (size %d)\n", num_keys);
    if (num_keys > 0) {
      for (uint32_t i = 0; i < num_keys; i++) {
        child = *node_internal_child(node, i);
        node_print_tree(pager, child, indentation_level + 1);
        for (uint32_t i = 0; i < indentation_level + 1; i++) {
          printf("  ");
        }
        printf("- key %d\n", *node_internal_key(node, i));
      }
      child = *node_internal_right_child(node);
      node_print_tree(pager, child, indentation_level + 1);
    }
    break;
  }
}
