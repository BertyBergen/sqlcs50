#include <fcntl.h>    
#include <stdint.h>   
#include <stdlib.h>   
#include <stdio.h>    
#include <unistd.h> 
#include "../include/table.h"
#include "../include/btree.h"


Database *database_open(const char *filename)
{
    Pager *pager = pager_open(filename);

    Database *db = malloc(sizeof(Database));
    db->pager = pager;

    if (pager->num_pages == 0)
    {
        db->schema.table_count = 0;
        db->schema.free_page_head = 0;
        void *schema_page = get_page(pager, SCHEMA_PAGE);
        memcpy(schema_page, &(db->schema), sizeof(DatabaseSchema));
    }
    else
    {
        void *schema_page = get_page(pager, SCHEMA_PAGE);
        memcpy(&(db->schema), schema_page, sizeof(DatabaseSchema));
    }
    return db;
}

void database_create_table(Database *db, const char *name)
{


    if (db->schema.table_count >= MAX_TABLES)
    {
        printf("Too many tables.\n");
        exit(EXIT_FAILURE);
    }
    uint32_t root_page;
    if (db->schema.free_page_head)
    {
        root_page = db->schema.free_page_head;
        void *free_page = get_page(db->pager, root_page);
        db->schema.free_page_head = *(uint32_t*)free_page;
    }

    else
    {
        root_page = db->pager->num_pages;
    }

    void *root_node = get_page(db->pager, root_page);
    initialize_leaf_node(root_node);
    set_node_root(root_node, true);
    TableMetadata *meta = &db->schema.tables[db->schema.table_count];
    strncpy(meta->name, name, MAX_TABLE_NAME);
    meta->root_page_num = root_page;
    db->schema.table_count++;

    void *schema_page =get_page(db->pager, SCHEMA_PAGE);
    memcpy(schema_page, &(db->schema), sizeof(DatabaseSchema));

}

Table *database_get_table(Database *db, const char *name)
{
    for (uint32_t i = 0; i < db->schema.table_count; i++)
    {
        if (strcmp(db->schema.tables[i].name, name) == 0)
        {
            Table *table = malloc(sizeof(Table));
            table->pager = db->pager;
            table->root_page_num = db->schema.tables[i].root_page_num;
            return table;
        }
    }
    return NULL;
}


void database_close(Database *db) 
{
    // Сохраняем schema
    Pager *pager = db->pager;
    void *schema_page = get_page(pager, SCHEMA_PAGE);
    memcpy(schema_page, &db->schema, sizeof(DatabaseSchema));
    pager_flush(pager, 0);

    // Освобождаем память
    for (uint32_t i = 0; i < pager->num_pages; i++) 
    {
        if (pager->pages[i]) {
            pager_flush(pager, i);
            free(pager->pages[i]);
            pager->pages[i] = NULL;
        }
    }
    
    if (ftruncate(pager->file_descriptor, pager->num_pages * PAGE_SIZE) == -1) 
    {
        printf("Error truncating file: %d\n", errno); // errno global variable for save error message
    }  

    int result = close(pager->file_descriptor);
    
    if (result == -1) 
    {
        printf("Error closing db file.\n");
        exit(EXIT_FAILURE);
    }
    
    free(pager);
    free(db);
}

bool database_drop_table(Database *db, const char *table_name) 
{
    DatabaseSchema *schema = &db->schema;

    int idx = -1;
    
    for (int i = 0; i < schema->table_count; i++) 
    {
        if (strcmp(schema->tables[i].name, table_name) == 0) 
        {
            set_page_free(db, db->schema.tables->root_page_num);
            idx = i;
            break;
        }
    }
    
    if (idx == -1) 
    {
        return false; // Таблица не найдена
    }

    // Сдвигаем остальные таблицы
    for (int i = idx; i < schema->table_count - 1; i++) 
    {
        schema->tables[i] = schema->tables[i + 1];
    }
    schema->table_count--;

    // Записываем обратно схему в страницу 0
    void *schema_page = get_page(db->pager, 0);
    memcpy(schema_page, schema, sizeof(DatabaseSchema));

    // TODO: освободить страницы таблицы (если есть механика)
    return true;
}

void add_page_to_freelist(Database *db, uint32_t page_num) 
{
    void* page = get_page(db->pager, page_num);
    memset(page, 0, PAGE_SIZE); // Clear whole list
    *((uint32_t*)page) = db->schema.free_page_head; // Next free sheet
    db->schema.free_page_head = page_num;           // update list head
}

uint32_t get_freelsit_page(Database *db)
{
    uint32_t page_num = db->schema.free_page_head;
    void* page = get_page(db->pager, page_num);
    db->schema.free_page_head = *((uint32_t*)page); // next free
    return page_num;
}

void set_page_free(Database *db, uint32_t page_num) 
{
    void *node = get_page(db->pager, page_num);
    uint32_t num_keys, child;
    
    switch (get_node_type(node)) 
    {
        case (NODE_LEAF):
            
            add_page_to_freelist(db, page_num);            
            break;

        case (NODE_INTERNAL):
            num_keys = *internal_node_num_keys(node);
            printf("- internal (size %d)\n", num_keys);
            for (uint32_t i = 0; i < num_keys; i++) 
            {
                child = *internal_node_child(node, i);
                set_page_free(db, child);
            
            }
            child = *internal_node_right_child(node);

            set_page_free(db, child);
            add_page_to_freelist(db, page_num);
            break;
    }
}
