#ifndef TABLE_H
#define TABLE_H

#include <stdint.h>
#include <stdbool.h>
#include "pager.h"

typedef struct Table {
    Pager *pager;
    uint32_t root_page_num;
} Table;

typedef struct {
    char name[MAX_TABLE_NAME + 1];
    uint32_t root_page_num;
} TableMetadata;

typedef struct {
    TableMetadata tables[MAX_TABLES];
    uint32_t table_count;
    uint32_t free_page_head;
} DatabaseSchema;

typedef struct {
    Pager *pager;
    DatabaseSchema schema;
    Table *current_table;
} Database;

Database *database_open(const char *filename);
void database_create_table(Database *db, const char *name);
Table *database_get_table(Database *db, const char *name);
void print_row(Row* row);
void database_close(Database *db);
bool database_drop_table(Database *db, const char *table_name);

void add_page_to_freelist(Database* db, uint32_t page_num);
uint32_t get_freelsit_page(Database *db);
void set_page_free(Database *db, uint32_t page_num);

#endif
 