#ifndef TABLE_H
#define TABLE_H

#include <stdint.h>
#include "pager.h"
#include "row.h"

typedef struct {
    Pager *pager;
    uint32_t num_rows;
} Table;

Table* db_open(const char *filename);
void print_row(Row* row);
void db_close(Table* table) ;

#endif
