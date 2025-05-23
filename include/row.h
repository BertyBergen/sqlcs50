#ifndef ROW_H
#define ROW_H

#include <stdint.h>
#include "constants.h"


typedef enum { NODE_INTERNAL, NODE_LEAF} NodeType;

typedef struct f{// Layout  Каждая строка (Row) занимает строго фиксированный размер: id (4 байта), username (32), email (255).
    uint32_t id; // Используем оффсеты для записи/чтения: ID_OFFSET, USERNAME_OFFSET и т.д.
    char username[COLUMN_USERNAME_SIZE + 1]; // Такой layout нужен для бинарной сериализации. \0
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
extern const uint32_t LEAF_NODE_NEXT_LEAF_SIZE;
extern const uint32_t LEAF_NODE_NEXT_LEAF_OFFSET; 
extern const uint32_t LEAF_NODE_HEADER_SIZE;

extern const uint32_t LEAF_NODE_KEY_SIZE;
extern const uint32_t INTERNAL_NODE_NUM_KEYS_OFFSET;
extern const uint32_t LEAF_NODE_KEY_OFFSET;
extern const uint32_t LEAF_NODE_VALUE_SIZE;
extern const uint32_t LEAF_NODE_VALUE_OFFSET;

extern const uint32_t LEAF_NODE_CELL_SIZE;
extern const uint32_t LEAF_NODE_SPACE_FOR_CELLS;
extern const uint32_t LEAF_NODE_MAX_CELLS;

extern const uint32_t LEAF_NODE_RIGHT_SPLIT_COUNT;
extern const uint32_t LEAF_NODE_LEFT_SPLIT_COUNT;
extern const uint32_t INTERNAL_NODE_RIGHT_CHILD_SIZE;
extern const uint32_t INTERNAL_NODE_RIGHT_CHILD_OFFSET;
extern const uint32_t INTERNAL_NODE_HEADER_SIZE;
extern const uint32_t INTERNAL_NODE_KEY_SIZE;
extern const uint32_t INTERNAL_NODE_CHILD_SIZE;
extern const uint32_t INTERNAL_NODE_CELL_SIZE;
extern const uint32_t INTERNAL_NODE_MAX_KEYS;


void serialize_row(Row *source, void *destination);
void deserialize_row(void *source, Row *destination);
void print_row(Row *row);
void print_constants();
void indent(uint32_t level);


#endif
