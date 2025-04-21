#ifndef CURSOR_H
#define CURSOR_H

#include <stdbool.h>
#include <stdint.h>
// #include "table.h"
// #include "row.h"

struct Table;

typedef struct {
    struct Table *table;
    // uint32_t row_num;
    uint32_t page_num;
    uint32_t cell_num;
    bool end_of_table;
} Cursor;


Cursor *table_start(struct Table *table);
Cursor* table_find(struct Table* table, uint32_t key);
Cursor* leaf_node_find(struct Table* table, uint32_t page_num, uint32_t key);
void* cursor_value(Cursor* cursor);
void cursor_advance(Cursor* cursor);
uint32_t internal_node_find_child(void *node, uint32_t key);
Cursor* internal_node_find(struct Table* table, uint32_t page_num, uint32_t key);
void internal_node_insert(struct Table* table, uint32_t parent_page_num, uint32_t child_page_num);

#endif