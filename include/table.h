#ifndef TABLE_H
#define TABLE_H

#include <stdint.h>
#include "pager.h"

typedef struct Table {
    Pager *pager;
    uint32_t root_page_num;
} Table;

typedef struct {
    char name[32];
    uint32_t root_page_num;
} TableMetadata;

typedef struct {
    TableMetadata table_metadatas[64];
    uint32_t table_count;
} DatabaseSchema;

Table* db_open(const char *filename);
void print_row(Row* row);
void db_close(Table* table) ;
int find_table_by_name(const char *name, DatabaseSchema *schema);

#endif
