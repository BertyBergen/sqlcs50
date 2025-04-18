#ifndef ROW_H
#define ROW_H

#include <stdint.h>
#include "cursor.h"
// #include "pager.h"
#include "constants.h"


typedef enum { NODE_INTERNAL, NODE_LEAF} NodeType;

typedef struct {// Layout  Каждая строка (Row) занимает строго фиксированный размер: id (4 байта), username (32), email (255).
    uint32_t id; // Используем оффсеты для записи/чтения: ID_OFFSET, USERNAME_OFFSET и т.д.
    char username[COLUMN_USERNAME_SIZE + 1]; // Такой layout нужен для бинарной сериализации.
    char email[COLUMN_EMAIL_SIZE + 1];
} Row;  


extern const uint32_t ID_SIZE;
extern const uint32_t USERNAME_SIZE;
extern const uint32_t EMAIL_SIZE;

extern const uint32_t ID_OFFSET;
extern const uint32_t USERNAME_OFFSET;
extern const uint32_t EMAIL_OFFSET;
extern const uint32_t ROW_SIZE;

extern const uint32_t ROWS_PER_PAGE;
extern const uint32_t TABLE_MAX_ROWS;

extern const uint32_t NODE_TYPE_SIZE; 
extern const uint32_t NODE_TYPE_OFFSET; 
extern const uint32_t IS_ROOT_SIZE; 
extern const uint32_t IS_ROOT_OFFSET; 
extern const uint32_t PARENT_POINTER_SIZE; 
extern const uint32_t PARENT_POINTER_OFFSET; 
extern const uint8_t COMMON_NODE_HEADER_SIZE; 

extern const uint32_t LEAF_NODE_NUM_CELLS_SIZE;
extern const uint32_t LEAF_NODE_NUM_CELLS_OFFSET;
extern const uint32_t LEAF_NODE_HEADER_SIZE;

extern const uint32_t LEAF_NODE_KEY_SIZE;
extern const uint32_t LEAF_NODE_KEY_OFFSET;
extern const uint32_t LEAF_NODE_VALUE_SIZE;
extern const uint32_t LEAF_NODE_VALUE_OFFSET;

extern const uint32_t LEAF_NODE_CELL_SIZE;
extern const uint32_t LEAF_NODE_SPACE_FOR_CELLS;
extern const uint32_t LEAF_NODE_MAX_CELLS;


void serialize_row(Row *source, void *destination);
void deserialize_row(void *source, Row *destination);

uint32_t *leaf_node_num_cells(void *node);
void *leaf_node_cell(void *node, uint32_t cell_num);
uint32_t *leaf_node_key(void *node, uint32_t cell_num);
void *leaf_node_value(void *node, uint32_t cell_num);
void initialize_leaf_node(void *node); 
void leaf_node_insert(Cursor *cursor, uint32_t key, Row* value);
void print_row(Row *row);
void print_constants();
void indent(uint32_t level);
NodeType get_node_type(void *node);
void set_node_type(void *node, NodeType type);
void leaf_node_split_and_insert(Cursor *cursor, uint32_t key, Row* value);
uint32_t *internal_node_num_keys(void *node);
uint32_t *internal_node_right_child(void *node);
uint32_t *internal_node_cell(void *node, uint32_t cell_num);
uint32_t *internal_node_child(void *node, uint32_t child_num);
uint32_t *internal_node_key(void *node, uint32_t key_num);
uint32_t get_node_max_key(void *node);
bool is_node_root(void *node);
void set_node_root(void *node, bool is_root);
void initialize_internal_node(void *node);
void create_new_root(struct Table *table, uint32_t right_child_page_num);


#endif
