#ifndef NODE_H
#define NODE_H

#include "cursor.h"
#include "pager.h"
#include "row.h"
#include "table.h"
#include <stdbool.h>
#include <stdint.h>

// NodeType is an enum that represents the type of a node in the B-tree
// Each node corresponds to a page in the database file
typedef enum { NODE_TYPE_INTERNAL, NODE_TYPE_LEAF } NodeType;

// Common Node Header Layout
// Nodes need to store metadata in a header at the beginning of the page, e.g.
// the type of node, whether or not it is the root node, and a pointer to its
// parent (to allow finding a node’s siblings)
static const uint32_t NODE_TYPE_SIZE = sizeof(uint8_t);
static const uint32_t NODE_TYPE_OFFSET = 0;
static const uint32_t NODE_IS_ROOT_SIZE = sizeof(uint8_t);
static const uint32_t NODE_IS_ROOT_OFFSET = NODE_TYPE_SIZE;
static const uint32_t NODE_PARENT_POINTER_SIZE = sizeof(uint32_t);
static const uint32_t NODE_PARENT_POINTER_OFFSET =
    NODE_IS_ROOT_OFFSET + NODE_IS_ROOT_SIZE;
static const uint8_t NODE_COMMON_HEADER_SIZE =
    NODE_TYPE_SIZE + NODE_IS_ROOT_SIZE + NODE_PARENT_POINTER_SIZE;

// Leaf Node Header Layout
// A cell is a key-value pair, where the value is a serialized row. The body of
// a leaf node consists of a number of cells.
static const uint32_t NODE_LEAF_NUM_CELLS_SIZE = sizeof(uint32_t);
static const uint32_t NODE_LEAF_NUM_CELLS_OFFSET = NODE_COMMON_HEADER_SIZE;
static const uint32_t NODE_LEAF_NEXT_LEAF_SIZE = sizeof(uint32_t);
static const uint32_t NODE_LEAF_NEXT_LEAF_OFFSET =
    NODE_LEAF_NUM_CELLS_OFFSET + NODE_LEAF_NUM_CELLS_SIZE;
static const uint32_t NODE_LEAF_HEADER_SIZE = NODE_COMMON_HEADER_SIZE +
                                              NODE_LEAF_NUM_CELLS_SIZE +
                                              NODE_LEAF_NEXT_LEAF_SIZE;

// Internal Node Header Layout
static const uint32_t NODE_INTERNAL_NUM_KEYS_SIZE = sizeof(uint32_t);
static const uint32_t NODE_INTERNAL_NUM_KEYS_OFFSET = NODE_COMMON_HEADER_SIZE;
static const uint32_t NODE_INTERNAL_RIGHT_CHILD_SIZE = sizeof(uint32_t);
static const uint32_t NODE_INTERNAL_RIGHT_CHILD_OFFSET =
    NODE_INTERNAL_NUM_KEYS_OFFSET + NODE_INTERNAL_NUM_KEYS_SIZE;
static const uint32_t NODE_INTERNAL_HEADER_SIZE =
    NODE_COMMON_HEADER_SIZE + NODE_INTERNAL_NUM_KEYS_SIZE +
    NODE_INTERNAL_RIGHT_CHILD_SIZE;

// Leaf Node Body Layout
static const uint32_t NODE_LEAF_KEY_SIZE = sizeof(uint32_t);
static const uint32_t NODE_LEAF_KEY_OFFSET = 0;
static const uint32_t NODE_LEAF_VALUE_SIZE = ROW_SIZE;
static const uint32_t NODE_LEAF_VALUE_OFFSET =
    NODE_LEAF_KEY_OFFSET + NODE_LEAF_KEY_SIZE;
static const uint32_t NODE_LEAF_CELL_SIZE =
    NODE_LEAF_KEY_SIZE + NODE_LEAF_VALUE_SIZE;
static const uint32_t NODE_LEAF_SPACE_FOR_CELLS =
    PAGER_PAGE_SIZE - NODE_LEAF_HEADER_SIZE;
static const uint32_t NODE_LEAF_MAX_CELLS =
    NODE_LEAF_SPACE_FOR_CELLS / NODE_LEAF_CELL_SIZE;

// Internal Node Body Layout
// Each internal node can store 510 keys and 511 children
static const uint32_t NODE_INTERNAL_KEY_SIZE = sizeof(uint32_t);
static const uint32_t NODE_INTERNAL_CHILD_SIZE = sizeof(uint32_t);
static const uint32_t NODE_INTERNAL_CELL_SIZE =
    NODE_INTERNAL_CHILD_SIZE + NODE_INTERNAL_KEY_SIZE;
static const uint32_t NODE_INTERNAL_MAX_CELLS = 3;

// Leaf Node Split Configuration
static const uint32_t NODE_LEAF_RIGHT_SPLIT_COUNT =
    (NODE_LEAF_MAX_CELLS + 1) / 2;
static const uint32_t NODE_LEAF_LEFT_SPLIT_COUNT =
    (NODE_LEAF_MAX_CELLS + 1) - NODE_LEAF_RIGHT_SPLIT_COUNT;

// node_leaf_num_cells returns the number of cells in a leaf node
uint32_t *node_leaf_num_cells(void *node);

// node_leaf_cell returns a pointer to a cell in a leaf node
void *node_leaf_cell(void *node, uint32_t cell_num);

// node_leaf_key returns a pointer to the key of a cell in a leaf node
uint32_t *node_leaf_key(void *node, uint32_t cell_num);

// node_leaf_value returns a pointer to the value of a cell in a leaf node
void *node_leaf_value(void *node, uint32_t cell_num);

// node_leaf_initialize initializes a leaf node
void node_leaf_initialize(void *node);

// node_leaf_insert inserts a row into a leaf node
void node_leaf_insert(Cursor *cursor, uint32_t key, Row *value);

// node_leaf_find returns a cursor to a leaf node containing the given key
Cursor *node_leaf_find(Table *table, uint32_t page_num, uint32_t key);

// node_get_type returns the type of a node
NodeType node_get_type(void *node);

// node_set_type sets the type of a node
void node_set_type(void *node, NodeType type);

// node_leaf_split_and_insert splits a leaf node and inserts a new row
void node_leaf_split_and_insert(Cursor *cursor, uint32_t key, Row *value);

// node_leaf_next returns the page number of the next leaf node
uint32_t *node_leaf_next(void *node);

// node_create_new_root creates a new root node
void node_create_new_root(Table *table, uint32_t right_child_page_num);

// node_internal_num_keys returns the number of keys in an internal node
uint32_t *node_internal_num_keys(void *node);

// node_internal_right_child returns a pointer to the right child of an
uint32_t *node_internal_right_child(void *node);

// node_internal_cell returns a pointer to a cell in an internal node
uint32_t *node_internal_cell(void *node, uint32_t cell_num);

// node_internal_child returns a pointer to a child in an internal node
uint32_t *node_internal_child(void *node, uint32_t child_num);

// node_internal_key returns a pointer to the key of a cell in an
uint32_t *node_internal_key(void *node, uint32_t key_num);

// node_get_max_key returns the maximum key in a node
uint32_t node_get_max_key(void *node);

// node_is_root returns whether or not a node is the root node
bool node_is_root(void *node);

// node_set_root sets whether or not a node is the root node
void node_set_root(void *node, bool is_root);

// node_internal_initialize initializes an internal node
void node_internal_initialize(void *node);

// node_internal_find returns a cursor to an internal node containing the given
// key
Cursor *node_internal_find(Table *table, uint32_t page_num, uint32_t key);

// node_internal_find_child returns the index of a child in an internal node
uint32_t node_internal_find_child(void *node, uint32_t key);

// node_internal_insert inserts a child into an internal node
void node_internal_insert(Table *table, uint32_t parent_page_num,
                          uint32_t child_page_num);

// node_parent returns a pointer to the parent of a node
uint32_t *node_parent(void *node);

// node_internal_update_key updates the key of a child in an internal node
void node_internal_update_key(void *node, uint32_t old_key, uint32_t new_key);

// node_print_tree prints the btree to stdout
void node_print_tree(Pager *pager, uint32_t page_num,
                     uint32_t indentation_level);

#endif
