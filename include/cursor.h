#ifndef CURSOR_H
#define CURSOR_H

#include <stdbool.h>
#include <stdint.h>
// #include "table.h"

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
Cursor* internal_node_find(struct Table* table, uint32_t page_num, uint32_t key);

#endif