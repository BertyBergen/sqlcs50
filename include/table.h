#ifndef TABLE_H
#define TABLE_H

#include <stdint.h>
#include "pager.h"

typedef struct Table {
    Pager *pager;
    uint32_t root_page_num;
} Table;

Table* db_open(const char *filename);
void print_row(Row* row);
void db_close(Table* table) ;

#endif
