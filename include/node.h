#ifndef NODE_H
#define NODE_H

#include "cursor.h"
#include "pager.h"
#include "row.h"
#include <stdint.h>

// NodeType is an enum that represents the type of a node in the B-tree
// Each node corresponds to a page in the database file
typedef enum { NODE_INTERNAL, NODE_LEAF } NodeType;

// Common Node Header Layout
// Nodes need to store metadata in a header at the beginning of the page, e.g.
// the type of node, whether or not it is the root node, and a pointer to its
// parent (to allow finding a node’s siblings)
static const uint32_t NODE_TYPE_SIZE = sizeof(uint8_t);
static const uint32_t NODE_TYPE_OFFSET = 0;
static const uint32_t IS_ROOT_SIZE = sizeof(uint8_t);
static const uint32_t IS_ROOT_OFFSET = NODE_TYPE_SIZE;
static const uint32_t PARENT_POINTER_SIZE = sizeof(uint32_t);
static const uint32_t PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;
static const uint8_t COMMON_NODE_HEADER_SIZE =
    NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;

// Leaf Node Header Layout
// A cell is a key-value pair, where the value is a serialized row. The body of
// a leaf node consists of a number of cells.
static const uint32_t LEAF_NODE_NUM_CELLS_SIZE = sizeof(uint32_t);
static const uint32_t LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
static const uint32_t LEAF_NODE_HEADER_SIZE =
    COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE;

// Leaf Node Body Layout
static const uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);
static const uint32_t LEAF_NODE_KEY_OFFSET = 0;
static const uint32_t LEAF_NODE_VALUE_SIZE = ROW_SIZE;
static const uint32_t LEAF_NODE_VALUE_OFFSET =
    LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;
static const uint32_t LEAF_NODE_CELL_SIZE =
    LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
static const uint32_t LEAF_NODE_SPACE_FOR_CELLS =
    PAGER_PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
static const uint32_t LEAF_NODE_MAX_CELLS =
    LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;

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

// node_leaf_print prints the btree to stdout
void node_leaf_print(void *node);

#endif
