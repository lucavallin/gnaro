#include "btree.h"
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

uint32_t *btree_node_leaf_num_cells(void *node) {
  log_debug("getting num_cells from node...");
  return node + BTREE_NODE_LEAF_NUM_CELLS_OFFSET;
}

void *btree_node_leaf_cell(void *node, uint32_t cell_num) {
  log_debug("getting cell %d from node...", cell_num);
  return node + BTREE_NODE_LEAF_HEADER_SIZE +
         (size_t)(cell_num * BTREE_NODE_LEAF_CELL_SIZE);
}

uint32_t *btree_node_leaf_key(void *node, uint32_t cell_num) {
  log_debug("getting key from cell %d...", cell_num);
  return btree_node_leaf_cell(node, cell_num);
}

void *btree_node_leaf_value(void *node, uint32_t cell_num) {
  log_debug("getting value from cell %d...", cell_num);
  return btree_node_leaf_cell(node, cell_num) + BTREE_NODE_LEAF_KEY_SIZE;
}

void btree_node_leaf_init(void *node) {
  log_debug("initializing leaf node...");
  btree_node_set_type(node, BTREE_NODE_TYPE_LEAF);
  btree_node_set_root(node, false);
  *btree_node_leaf_num_cells(node) = 0;
  // Set the next leaf to 0 (no sibling)
  *btree_node_leaf_next(node) = 0;
}

void btree_node_leaf_insert(Cursor *cursor, uint32_t key, Row *value) {
  log_debug("inserting row into node...");
  void *node = pager_get_page(cursor->database->pager, cursor->page_num);

  uint32_t num_cells = *btree_node_leaf_num_cells(node);
  if (num_cells >= BTREE_NODE_LEAF_MAX_CELLS) {
    btree_node_leaf_split_and_insert(cursor, key, value);
    return;
  }

  if (cursor->cell_num < num_cells) {
    log_debug("making room for new cell...");
    for (uint32_t i = num_cells; i > cursor->cell_num; i--) {
      log_debug("moving cell %d to the right...", i);
      memcpy(btree_node_leaf_cell(node, i), btree_node_leaf_cell(node, i - 1),
             BTREE_NODE_LEAF_CELL_SIZE);
    }
  }

  log_debug("incrementing num_cells...");
  *(btree_node_leaf_num_cells(node)) += 1;

  log_debug("setting key...");
  *(btree_node_leaf_key(node, cursor->cell_num)) = key;

  log_debug("serializing row...");
  row_serialize(value, btree_node_leaf_value(node, cursor->cell_num));

  log_debug("row inserted into node");
}

// Returns the position of the key, or the position of another key to move to
// for inserting the new key, or the position one past the last key
Cursor *btree_node_leaf_find(Database *database, uint32_t page_num,
                             uint32_t key) {
  log_debug("finding key %d in node...", key);
  void *node = pager_get_page(database->pager, page_num);
  uint32_t num_cells = *btree_node_leaf_num_cells(node);

  log_debug("allocating cursor...");
  Cursor *cursor = malloc(sizeof(Cursor));
  cursor->database = database;
  cursor->page_num = page_num;

  log_debug("binary searching for key %d...", key);
  uint32_t min_index = 0;
  uint32_t one_past_max_index = num_cells;
  while (one_past_max_index != min_index) {
    log_debug("min_index: %d, one_past_max_index: %d...", min_index,
              one_past_max_index);
    uint32_t index = (min_index + one_past_max_index) / 2;
    uint32_t key_at_index = *btree_node_leaf_key(node, index);

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

NodeType btree_node_get_type(void *node) {
  log_debug("getting node type...");
  // Cast to uint8_t to ensure it is serialized as a single byte
  uint8_t value = *((uint8_t *)(node + BTREE_NODE_TYPE_OFFSET));
  return (NodeType)value;
}

void btree_node_set_type(void *node, NodeType type) {
  log_debug("setting node type to %d...", type);
  uint8_t value = type;
  *((uint8_t *)(node + BTREE_NODE_TYPE_OFFSET)) = value;
}

void btree_node_leaf_split_and_insert(Cursor *cursor, uint32_t key,
                                      Row *value) {
  // Create a new node and move half the cells over.
  // Insert the new value in one of the two nodes.
  // Update parent or create a new parent.
  log_debug("splitting node and inserting row...");
  void *old_node = pager_get_page(cursor->database->pager, cursor->page_num);
  uint32_t old_max = btree_node_get_max_key(cursor->database->pager, old_node);
  uint32_t new_page_num = pager_get_unused_page_num(cursor->database->pager);
  void *new_node = pager_get_page(cursor->database->pager, new_page_num);

  log_debug("initializing new node...");
  btree_node_leaf_init(new_node);
  *btree_node_parent(new_node) = *btree_node_parent(old_node);
  *btree_node_leaf_next(new_node) = *btree_node_leaf_next(old_node);
  *btree_node_leaf_next(old_node) = new_page_num;

  log_debug("dividing keys evenly between old (left) and new (right) nodes...");
  for (int32_t i = (int32_t)BTREE_NODE_LEAF_MAX_CELLS; i >= 0; i--) {
    log_debug("moving cell %d...", i);

    void *destination_node;
    if (i >= (int32_t)BTREE_NODE_LEAF_LEFT_SPLIT_COUNT) {
      destination_node = new_node;
    } else {
      destination_node = old_node;
    }
    uint32_t index_within_node = i % BTREE_NODE_LEAF_LEFT_SPLIT_COUNT;
    void *destination =
        btree_node_leaf_cell(destination_node, index_within_node);

    if (i == (int32_t)cursor->cell_num) {
      log_debug("inserting row into new node...");
      row_serialize(value,
                    btree_node_leaf_value(destination_node, index_within_node));
      *btree_node_leaf_key(destination_node, index_within_node) = key;
    } else if (i > (int32_t)cursor->cell_num) {
      memcpy(destination, btree_node_leaf_cell(old_node, i - 1),
             BTREE_NODE_LEAF_CELL_SIZE);
    } else {
      memcpy(destination, btree_node_leaf_cell(old_node, i),
             BTREE_NODE_LEAF_CELL_SIZE);
    }
  }

  log_debug("updating cell counts...");
  *(btree_node_leaf_num_cells(old_node)) = BTREE_NODE_LEAF_LEFT_SPLIT_COUNT;
  *(btree_node_leaf_num_cells(new_node)) = BTREE_NODE_LEAF_RIGHT_SPLIT_COUNT;

  log_debug("updating parent node...");
  if (btree_node_is_root(old_node)) {
    return btree_node_new_root(cursor->database, new_page_num);
  }

  uint32_t parent_page_num = *btree_node_parent(old_node);
  uint32_t new_max = btree_node_get_max_key(cursor->database->pager, old_node);
  void *parent = pager_get_page(cursor->database->pager, parent_page_num);

  btree_node_internal_update_key(parent, old_max, new_max);
  btree_node_internal_insert(cursor->database, parent_page_num, new_page_num);
}

uint32_t *btree_node_leaf_next(void *node) {
  return node + BTREE_NODE_LEAF_NEXT_LEAF_OFFSET;
}

void btree_node_new_root(Database *database, uint32_t right_child_page_num) {
  // Handle splitting the root.
  // Old root copied to new page, becomes left child.
  // Address of right child passed in.
  // Re-initialize root page to contain the new root node.
  // New root node points to two children.
  log_debug("creating new root node...");
  void *root = pager_get_page(database->pager, database->root_page_num);
  void *right_child = pager_get_page(database->pager, right_child_page_num);
  uint32_t left_child_page_num = pager_get_unused_page_num(database->pager);
  void *left_child = pager_get_page(database->pager, left_child_page_num);

  if (btree_node_get_type(root) == BTREE_NODE_TYPE_INTERNAL) {
    btree_node_internal_init(right_child);
    btree_node_internal_init(left_child);
  }

  log_debug("copying old root to left child...");
  memcpy(left_child, root, PAGER_PAGE_SIZE);
  btree_node_set_root(left_child, false);

  if (btree_node_get_type(left_child) == BTREE_NODE_TYPE_INTERNAL) {
    void *child;
    for (uint32_t i = 0; i < *btree_node_internal_num_keys(left_child); i++) {
      child = pager_get_page(database->pager,
                             *btree_node_internal_child(left_child, i));
      *btree_node_parent(child) = left_child_page_num;
    }
    child = pager_get_page(database->pager,
                           *btree_node_internal_right_child(left_child));
    *btree_node_parent(child) = left_child_page_num;
  }

  // Root node is a new internal node with one key and two children
  log_debug("initializing new root node...");
  btree_node_internal_init(root);
  btree_node_set_root(root, true);
  *btree_node_internal_num_keys(root) = 1;
  *btree_node_internal_child(root, 0) = left_child_page_num;
  uint32_t left_child_max_key =
      btree_node_get_max_key(database->pager, left_child);
  *btree_node_internal_key(root, 0) = left_child_max_key;
  *btree_node_internal_right_child(root) = right_child_page_num;
  *btree_node_parent(left_child) = database->root_page_num;
  *btree_node_parent(right_child) = database->root_page_num;
}

uint32_t *btree_node_internal_num_keys(void *node) {
  log_debug("getting num_keys from node...");
  return node + BTREE_NODE_INTERNAL_NUM_KEYS_OFFSET;
}

uint32_t *btree_node_internal_right_child(void *node) {
  log_debug("getting right_child from node...");
  return node + BTREE_NODE_INTERNAL_RIGHT_CHILD_OFFSET;
}

uint32_t *btree_node_internal_cell(void *node, uint32_t cell_num) {
  log_debug("getting cell %d from node...", cell_num);
  return node + BTREE_NODE_INTERNAL_HEADER_SIZE +
         (size_t)(cell_num * BTREE_NODE_INTERNAL_CELL_SIZE);
}

uint32_t *btree_node_internal_child(void *node, uint32_t child_num) {
  log_debug("getting child %d from node...", child_num);
  uint32_t num_keys = *btree_node_internal_num_keys(node);

  if (child_num > num_keys) {
    log_error("failed to access child_num %d, max num_keys is %d\n", child_num,
              num_keys);
    exit(EXIT_FAILURE);
  } else if (child_num == num_keys) {
    log_debug("getting right_child from node...");
    uint32_t *right_child = btree_node_internal_right_child(node);
    if (*right_child == BTREE_NODE_INTERNAL_INVALID_PAGE_NUM) {
      log_error("right child of node is an invalid page");
      exit(EXIT_FAILURE);
    }
    return right_child;
  } else {
    log_debug("getting child %d from node...", child_num);
    uint32_t *child = btree_node_internal_cell(node, child_num);
    if (*child == BTREE_NODE_INTERNAL_INVALID_PAGE_NUM) {
      log_error("right child %d of node is an invalid page", child_num);
      exit(EXIT_FAILURE);
    }
    return child;
  }
}

uint32_t *btree_node_internal_key(void *node, uint32_t key_num) {
  log_debug("getting key %d from node...", key_num);
  return (void *)btree_node_internal_cell(node, key_num) +
         BTREE_NODE_INTERNAL_CHILD_SIZE;
}

uint32_t btree_node_get_max_key(Pager *pager, void *node) {
  if (btree_node_get_type(node) == BTREE_NODE_TYPE_LEAF) {
    return *btree_node_leaf_key(node, *btree_node_leaf_num_cells(node) - 1);
  }
  void *right_child =
      pager_get_page(pager, *btree_node_internal_right_child(node));
  return btree_node_get_max_key(pager, right_child);
}

bool btree_node_is_root(void *node) {
  log_debug("checking if node is root...");
  uint8_t value = *((uint8_t *)(node + BTREE_NODE_IS_ROOT_OFFSET));
  return (bool)value;
}

void btree_node_set_root(void *node, bool is_root) {
  log_debug("setting node root status to %d...", is_root);
  uint8_t value = is_root;
  *((uint8_t *)(node + BTREE_NODE_IS_ROOT_OFFSET)) = value;
}

void btree_node_internal_init(void *node) {
  log_debug("initializing internal node...");
  btree_node_set_type(node, BTREE_NODE_TYPE_INTERNAL);
  btree_node_set_root(node, false);
  *btree_node_internal_num_keys(node) = 0;
  log_debug("setting right child to invalid page number to avoid root parent "
            "bug...");
  *btree_node_internal_right_child(node) = BTREE_NODE_INTERNAL_INVALID_PAGE_NUM;
}

Cursor *btree_node_internal_find(Database *database, uint32_t page_num,
                                 uint32_t key) {
  log_debug("finding key %d in internal node...", key);
  void *node = pager_get_page(database->pager, page_num);
  uint32_t child_index = btree_node_internal_find_child(node, key);
  uint32_t child_num = *btree_node_internal_child(node, child_index);
  void *child = pager_get_page(database->pager, child_num);

  switch (btree_node_get_type(child)) {
  case BTREE_NODE_TYPE_LEAF:
    return btree_node_leaf_find(database, child_num, key);
  case BTREE_NODE_TYPE_INTERNAL:
    return btree_node_internal_find(database, child_num, key);
  }
}

uint32_t btree_node_internal_find_child(void *node, uint32_t key) {
  log_debug("finding child for key %d...", key);
  uint32_t num_keys = *btree_node_internal_num_keys(node);

  log_debug("binary searching for key %d...", key);
  uint32_t min_index = 0;
  uint32_t max_index = num_keys;

  while (min_index != max_index) {
    log_debug("min_index: %d, max_index: %d...", min_index, max_index);
    uint32_t index = (min_index + max_index) / 2;
    uint32_t key_to_right = *btree_node_internal_key(node, index);
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

uint32_t *btree_node_parent(void *node) {
  log_debug("getting parent from node...");
  return node + BTREE_NODE_PARENT_POINTER_OFFSET;
}

void btree_node_internal_update_key(void *node, uint32_t old_key,
                                    uint32_t new_key) {
  uint32_t old_child_index = btree_node_internal_find_child(node, old_key);
  *btree_node_internal_key(node, old_child_index) = new_key;
}

void btree_node_internal_insert(Database *database, uint32_t parent_page_num,
                                uint32_t child_page_num) {
  log_debug("inserting new child into internal node...");
  void *parent = pager_get_page(database->pager, parent_page_num);
  void *child = pager_get_page(database->pager, child_page_num);
  uint32_t child_max_key = btree_node_get_max_key(database->pager, child);
  uint32_t index = btree_node_internal_find_child(parent, child_max_key);

  uint32_t original_num_keys = *btree_node_internal_num_keys(parent);
  *btree_node_internal_num_keys(parent) = original_num_keys + 1;

  if (original_num_keys >= BTREE_NODE_INTERNAL_MAX_CELLS) {
    btree_node_internal_split_and_insert(database, parent_page_num,
                                         child_page_num);
    return;
  }

  uint32_t right_child_page_num = *btree_node_internal_right_child(parent);

  if (right_child_page_num == BTREE_NODE_INTERNAL_INVALID_PAGE_NUM) {
    log_debug("node is empty...");
    *btree_node_internal_right_child(parent) = child_page_num;
    return;
  }

  void *right_child = pager_get_page(database->pager, right_child_page_num);

  log_debug("incrementing num_keys...");
  *btree_node_internal_num_keys(parent) = original_num_keys + 1;

  if (child_max_key > btree_node_get_max_key(database->pager, right_child)) {
    log_debug("replace right child...");
    *btree_node_internal_child(parent, original_num_keys) =
        right_child_page_num;
    *btree_node_internal_key(parent, original_num_keys) =
        btree_node_get_max_key(database->pager, right_child);
    *btree_node_internal_right_child(parent) = child_page_num;
  } else {
    log_debug("making room for new cell...");
    for (uint32_t i = original_num_keys; i > index; i--) {
      void *destination = btree_node_internal_cell(parent, i);
      void *source = btree_node_internal_cell(parent, i - 1);
      memcpy(destination, source, BTREE_NODE_INTERNAL_CELL_SIZE);
    }
    *btree_node_internal_child(parent, index) = child_page_num;
    *btree_node_internal_key(parent, index) = child_max_key;
  }
}

void btree_node_internal_split_and_insert(Database *database,
                                          uint32_t parent_page_num,
                                          uint32_t child_page_num) {
  log_debug("splitting internal node and inserting new child...");
  uint32_t old_page_num = parent_page_num;
  void *old_node = pager_get_page(database->pager, parent_page_num);
  uint32_t old_max = btree_node_get_max_key(database->pager, old_node);

  void *child = pager_get_page(database->pager, child_page_num);
  uint32_t child_max = btree_node_get_max_key(database->pager, child);

  uint32_t new_page_num = pager_get_unused_page_num(database->pager);

  log_debug("checking if node is root...");
  uint32_t splitting_root = btree_node_is_root(old_node);

  void *parent;
  void *new_node;
  if (splitting_root) {
    log_debug("splitting root node...");
    btree_node_new_root(database, new_page_num);
    parent = pager_get_page(database->pager, database->root_page_num);

    log_debug("updating old node to point to new root's left child...");
    old_page_num = *btree_node_internal_child(parent, 0);
    old_node = pager_get_page(database->pager, old_page_num);
  } else {
    log_debug("splitting non-root node...");
    parent = pager_get_page(database->pager, *btree_node_parent(old_node));
    new_node = pager_get_page(database->pager, new_page_num);
    btree_node_internal_init(new_node);
  }

  uint32_t *old_num_keys = btree_node_internal_num_keys(old_node);

  uint32_t cur_page_num = *btree_node_internal_right_child(old_node);
  void *cur = pager_get_page(database->pager, cur_page_num);

  log_debug("moving right child to new node...");
  btree_node_internal_insert(database, new_page_num, cur_page_num);
  *btree_node_parent(cur) = new_page_num;
  *btree_node_internal_right_child(old_node) =
      BTREE_NODE_INTERNAL_INVALID_PAGE_NUM;

  log_debug("moving keys and children to new node...");
  for (uint32_t i = BTREE_NODE_INTERNAL_MAX_CELLS - 1;
       i > BTREE_NODE_INTERNAL_MAX_CELLS / 2; i--) {
    log_debug("moving cell %d...", i);
    cur_page_num = *btree_node_internal_child(old_node, i);
    cur = pager_get_page(database->pager, cur_page_num);

    btree_node_internal_insert(database, new_page_num, cur_page_num);
    *btree_node_parent(cur) = new_page_num;

    (*old_num_keys)--;
  }

  log_debug("updating old node to point to new node's left child...");
  *btree_node_internal_right_child(old_node) =
      *btree_node_internal_child(old_node, *old_num_keys - 1);
  (*old_num_keys)--;

  log_debug(
      "determining which node should contain the child to be inserted...");
  uint32_t max_after_split = btree_node_get_max_key(database->pager, old_node);

  uint32_t destination_page_num =
      child_max < max_after_split ? old_page_num : new_page_num;

  log_debug("inserting new child into destination node...");
  btree_node_internal_split_and_insert(database, destination_page_num,
                                       child_page_num);
  *btree_node_parent(child) = destination_page_num;

  log_debug("updating parent node...");
  btree_node_internal_update_key(
      parent, old_max, btree_node_get_max_key(database->pager, old_node));

  if (!splitting_root) {
    log_debug("not a root node, updating parent...");
    btree_node_internal_insert(database, *btree_node_parent(old_node),
                               new_page_num);
    *btree_node_parent(new_node) = *btree_node_parent(old_node);
  }
}

void btree_print(Pager *pager, uint32_t page_num, uint32_t indent_level) {
  void *node = pager_get_page(pager, page_num);
  uint32_t num_keys;
  uint32_t child;

  switch (btree_node_get_type(node)) {
  case (BTREE_NODE_TYPE_LEAF):
    num_keys = *btree_node_leaf_num_cells(node);
    for (uint32_t i = 0; i < indent_level; i++) {
      printf("  ");
    }
    printf("- leaf (size %d)\n", num_keys);
    for (uint32_t i = 0; i < num_keys; i++) {
      for (uint32_t i = 0; i < indent_level + 1; i++) {
        printf("  ");
      }
      printf("- %d\n", *btree_node_leaf_key(node, i));
    }
    break;
  case (BTREE_NODE_TYPE_INTERNAL):
    num_keys = *btree_node_internal_num_keys(node);
    for (uint32_t i = 0; i < indent_level; i++) {
      printf("  ");
    }
    printf("- internal (size %d)\n", num_keys);
    if (num_keys > 0) {
      for (uint32_t i = 0; i < num_keys; i++) {
        child = *btree_node_internal_child(node, i);
        btree_print(pager, child, indent_level + 1);
        for (uint32_t i = 0; i < indent_level + 1; i++) {
          printf("  ");
        }
        printf("- key %d\n", *btree_node_internal_key(node, i));
      }
      child = *btree_node_internal_right_child(node);
      btree_print(pager, child, indent_level + 1);
    }
    break;
  }
}
