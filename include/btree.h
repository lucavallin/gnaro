#ifndef BTREE_H
#define BTREE_H

#include "cursor.h"
#include "database.h"
#include "pager.h"
#include "row.h"
#include <stdbool.h>
#include <stdint.h>

// NodeType is an enum that represents the type of a node in the B-tree
// Each node corresponds to a page in the database file
typedef enum { BTREE_NODE_TYPE_INTERNAL, BTREE_NODE_TYPE_LEAF } NodeType;

// Common Node Header Layout
// Nodes need to store metadata in a header at the beginning of the page, e.g.
// the type of node, whether or not it is the root node, and a pointer to its
// parent (to allow finding a node’s siblings)
static const uint32_t BTREE_NODE_TYPE_SIZE = sizeof(uint8_t);
static const uint32_t BTREE_NODE_TYPE_OFFSET = 0;
static const uint32_t BTREE_NODE_IS_ROOT_SIZE = sizeof(uint8_t);
static const uint32_t BTREE_NODE_IS_ROOT_OFFSET = BTREE_NODE_TYPE_SIZE;
static const uint32_t BTREE_NODE_PARENT_POINTER_SIZE = sizeof(uint32_t);
static const uint32_t BTREE_NODE_PARENT_POINTER_OFFSET =
    BTREE_NODE_IS_ROOT_OFFSET + BTREE_NODE_IS_ROOT_SIZE;
static const uint8_t BTREE_NODE_COMMON_HEADER_SIZE =
    BTREE_NODE_TYPE_SIZE + BTREE_NODE_IS_ROOT_SIZE +
    BTREE_NODE_PARENT_POINTER_SIZE;

// Leaf Node Header Layout
// A cell is a key-value pair, where the value is a serialized row. The body of
// a leaf node consists of a number of cells.
static const uint32_t BTREE_NODE_LEAF_NUM_CELLS_SIZE = sizeof(uint32_t);
static const uint32_t BTREE_NODE_LEAF_NUM_CELLS_OFFSET =
    BTREE_NODE_COMMON_HEADER_SIZE;
static const uint32_t BTREE_NODE_LEAF_NEXT_LEAF_SIZE = sizeof(uint32_t);
static const uint32_t BTREE_NODE_LEAF_NEXT_LEAF_OFFSET =
    BTREE_NODE_LEAF_NUM_CELLS_OFFSET + BTREE_NODE_LEAF_NUM_CELLS_SIZE;
static const uint32_t BTREE_NODE_LEAF_HEADER_SIZE =
    BTREE_NODE_COMMON_HEADER_SIZE + BTREE_NODE_LEAF_NUM_CELLS_SIZE +
    BTREE_NODE_LEAF_NEXT_LEAF_SIZE;

// Internal Node Header Layout
static const uint32_t BTREE_NODE_INTERNAL_NUM_KEYS_SIZE = sizeof(uint32_t);
static const uint32_t BTREE_NODE_INTERNAL_NUM_KEYS_OFFSET =
    BTREE_NODE_COMMON_HEADER_SIZE;
static const uint32_t BTREE_NODE_INTERNAL_RIGHT_CHILD_SIZE = sizeof(uint32_t);
static const uint32_t BTREE_NODE_INTERNAL_RIGHT_CHILD_OFFSET =
    BTREE_NODE_INTERNAL_NUM_KEYS_OFFSET + BTREE_NODE_INTERNAL_NUM_KEYS_SIZE;
static const uint32_t BTREE_NODE_INTERNAL_HEADER_SIZE =
    BTREE_NODE_COMMON_HEADER_SIZE + BTREE_NODE_INTERNAL_NUM_KEYS_SIZE +
    BTREE_NODE_INTERNAL_RIGHT_CHILD_SIZE;

// Leaf Node Body Layout
static const uint32_t BTREE_NODE_LEAF_KEY_SIZE = sizeof(uint32_t);
static const uint32_t BTREE_NODE_LEAF_KEY_OFFSET = 0;
static const uint32_t BTREE_NODE_LEAF_VALUE_SIZE = ROW_SIZE;
static const uint32_t BTREE_NODE_LEAF_VALUE_OFFSET =
    BTREE_NODE_LEAF_KEY_OFFSET + BTREE_NODE_LEAF_KEY_SIZE;
static const uint32_t BTREE_NODE_LEAF_CELL_SIZE =
    BTREE_NODE_LEAF_KEY_SIZE + BTREE_NODE_LEAF_VALUE_SIZE;
static const uint32_t BTREE_NODE_LEAF_SPACE_FOR_CELLS =
    PAGER_PAGE_SIZE - BTREE_NODE_LEAF_HEADER_SIZE;
static const uint32_t BTREE_NODE_LEAF_MAX_CELLS =
    BTREE_NODE_LEAF_SPACE_FOR_CELLS / BTREE_NODE_LEAF_CELL_SIZE;

// Internal Node Body Layout
// Each internal node can store 510 keys and 511 children
static const uint32_t BTREE_NODE_INTERNAL_KEY_SIZE = sizeof(uint32_t);
static const uint32_t BTREE_NODE_INTERNAL_CHILD_SIZE = sizeof(uint32_t);
static const uint32_t BTREE_NODE_INTERNAL_CELL_SIZE =
    BTREE_NODE_INTERNAL_CHILD_SIZE + BTREE_NODE_INTERNAL_KEY_SIZE;
static const uint32_t BTREE_NODE_INTERNAL_MAX_CELLS = 3;

// Leaf Node Split Configuration
static const uint32_t BTREE_NODE_LEAF_RIGHT_SPLIT_COUNT =
    (BTREE_NODE_LEAF_MAX_CELLS + 1) / 2;
static const uint32_t BTREE_NODE_LEAF_LEFT_SPLIT_COUNT =
    (BTREE_NODE_LEAF_MAX_CELLS + 1) - BTREE_NODE_LEAF_RIGHT_SPLIT_COUNT;

// Internal Node Split Configuration
static const uint32_t BTREE_NODE_INTERNAL_INVALID_PAGE_NUM = UINT32_MAX;

// Get the number of cells in a leaf node
uint32_t *btree_node_leaf_num_cells(void *node);

// Get a pointer to a cell in a leaf node
void *btree_node_leaf_cell(void *node, uint32_t cell_num);

// Get a pointer to the key of a cell in a leaf node
uint32_t *btree_node_leaf_key(void *node, uint32_t cell_num);

// Get a pointer to the value of a cell in a leaf node
void *btree_node_leaf_value(void *node, uint32_t cell_num);

// Initialize a leaf node
void btree_node_leaf_init(void *node);

// Insert a row into a leaf node
void btree_node_leaf_insert(Cursor *cursor, uint32_t key, Row *value);

// Get a cursor to a leaf node containing the given key
Cursor *btree_node_leaf_find(Database *database, uint32_t page_num,
                             uint32_t key);

// Get the type of a node
NodeType btree_node_get_type(void *node);

// Set the type of a node
void btree_node_set_type(void *node, NodeType type);

// Split a leaf node and inserts a new row
void btree_node_leaf_split_and_insert(Cursor *cursor, uint32_t key, Row *value);

// Get the page number of the next leaf node
uint32_t *btree_node_leaf_next(void *node);

// Create a new root node
void btree_node_new_root(Database *database, uint32_t right_child_page_num);

// Get the number of keys in an internal node
uint32_t *btree_node_internal_num_keys(void *node);

// Get a pointer to the right child of an
uint32_t *btree_node_internal_right_child(void *node);

// Get a pointer to a cell in an internal node
uint32_t *btree_node_internal_cell(void *node, uint32_t cell_num);

// Get a pointer to a child in an internal node
uint32_t *btree_node_internal_child(void *node, uint32_t child_num);

// Get a pointer to the key of a cell in an
uint32_t *btree_node_internal_key(void *node, uint32_t key_num);

// Get the maximum key in a node
uint32_t btree_node_get_max_key(Pager *pager, void *node);

// Get whether or not a node is the root node
bool btree_node_is_root(void *node);

// Set whether or not a node is the root node
void btree_node_set_root(void *node, bool is_root);

// Initialize an internal node
void btree_node_internal_init(void *node);

// Get a cursor to an internal node containing the given key
Cursor *btree_node_internal_find(Database *database, uint32_t page_num,
                                 uint32_t key);

// Get the index of a child in an internal node
uint32_t btree_node_internal_find_child(void *node, uint32_t key);

// Get a child into an internal node
void btree_node_internal_insert(Database *database, uint32_t parent_page_num,
                                uint32_t child_page_num);

// Split an internal node and inserts a new child
void btree_node_internal_split_and_insert(Database *database,
                                          uint32_t parent_page_num,
                                          uint32_t child_page_num);

// Get a pointer to the parent of a node
uint32_t *btree_node_parent(void *node);

// Get the key of a child in an internal node
void btree_node_internal_update_key(void *node, uint32_t old_key,
                                    uint32_t new_key);

// Printthe btree to stdout
void btree_print(Pager *pager, uint32_t page_num, uint32_t indent_level);

#endif
