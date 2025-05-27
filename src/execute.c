#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/execute.h"

ExecuteResult execute_insert(Statement *statement, Table *table) 
{

    Row *row_to_insert = &(statement->row_to_insert);
    uint32_t key_to_insert = row_to_insert->id;
    Cursor *cursor = table_find(table, key_to_insert);
    

    void* node = get_page(table->pager, cursor->page_num);
    uint32_t num_cells = (*leaf_node_num_cells(node));

    if (cursor->cell_num < num_cells) 
    {
        uint32_t key_at_index = *leaf_node_key(node, cursor->cell_num);
        if (key_at_index == key_to_insert) 
        {
            return EXECUTE_DUPLICATE_KEY;
        }
    }
    leaf_node_insert(cursor, row_to_insert->id, row_to_insert);
    
    free(cursor);

    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Table *table) 
{
    Cursor *cursor = table_start(table);
    
    Row row;
    while (!(cursor->end_of_table)) {
        
        deserialize_row(cursor_value(cursor), &row);
        if (!row.is_deleted)
        {
            print_row(&row);    
        }
        
        cursor_advance(cursor);
    }

    free(cursor);
    
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_delete(Statement *statement, Table *table)
{
    uint32_t id_to_delete = statement->id_to_delete;
    Cursor *cursor = table_find(table, id_to_delete);
    void* node = get_page(table->pager, cursor->page_num);
    uint32_t num_cells = (*leaf_node_num_cells(node));

    if (cursor->cell_num < num_cells) 
    {
        uint32_t key_at_index = *leaf_node_key(node, cursor->cell_num);
        if (key_at_index == id_to_delete) 
        {
            void* value = leaf_node_value(node, cursor->cell_num);
            Row* row = value;
            row->is_deleted = 1;
            printf("You deleted %d \n", key_at_index);
            return EXECUTE_SUCCESS;
        }
    }
    free(cursor);
    return EXECUTE_NO_ID;
}

ExecuteResult execute_update(Statement *statement, Table *table)
{
    Row *row_to_update = &(statement->row_to_insert);
    uint32_t key_to_update = row_to_update->id;
    Cursor *cursor = table_find(table, key_to_update);

    void* node = get_page(table->pager, cursor->page_num);
    uint32_t num_cells = (*leaf_node_num_cells(node));

    if (cursor->cell_num < num_cells) 
    {
        uint32_t key_at_index = *leaf_node_key(node, cursor->cell_num);
        if (key_at_index == key_to_update) 
        {
            void* value = leaf_node_value(node, cursor->cell_num);
            Row* row = value;
            strcpy(row->username, row_to_update->username);
            strcpy(row->email, row_to_update->email);
            printf("You updated %d \n", key_at_index);
            return EXECUTE_SUCCESS;
        }
    }

    free(cursor);
    
    return EXECUTE_NO_ID;
}
ExecuteResult execute_statement(Statement *statement, Table *table) 
{
    switch (statement->type) {
        case (STATEMENT_INSERT):
            return execute_insert(statement, table);
        case (STATEMENT_SELECT):
            return execute_select(table);
        case (STATEMENT_DELETE):
            return execute_delete(statement, table);
        case (STATEMENT_UPDATE):
            return execute_update(statement, table);
        default:
            return EXECUTE_FAILURE;
    }
}
