#ifndef BTREE_H
#define BTREE_H

#include "cursor.h"
#include "table.h"


uint32_t *leaf_node_num_cells(void *node);
uint32_t* leaf_node_next_leaf(void* node);

void *leaf_node_cell(void *node, uint32_t cell_num);
uint32_t *leaf_node_key(void *node, uint32_t cell_num);
void *leaf_node_value(void *node, uint32_t cell_num);
NodeType get_node_type(void *node);
void set_node_type(void *node, NodeType type);
uint32_t* node_parent(void *node);
uint32_t *internal_node_num_keys(void *node);
uint32_t *internal_node_right_child(void *node);
uint32_t *internal_node_cell(void *node, uint32_t cell_num);
uint32_t *internal_node_child(void *node, uint32_t child_num);
uint32_t *internal_node_key(void *node, uint32_t key_num);
void update_internal_node_key(void* node, uint32_t old_key, uint32_t new_key);
bool is_node_root(void *node);
void set_node_root(void *node, bool is_root);

void initialize_leaf_node(void *node);
void leaf_node_insert(Cursor *cursor, uint32_t key, Row *value);
void leaf_node_split_and_insert(Cursor* cursor, uint32_t key, Row* value);
void create_new_root(struct Table *table, uint32_t right_child_page_num);
void initialize_internal_node(void *node);


#endif
