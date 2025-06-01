#include <stdlib.h>
#include <string.h>
#include "../include/meta_commands.h"

MetaCommandResult do_meta_command(InputBuffer *input_buffer, Database *db) 
{
    if (strncmp(input_buffer->buffer, ".use", 4) == 0) 
    {
        char* table_name = input_buffer->buffer + 5; // пропускаем ".use "
        table_name[strcspn(table_name, "\n")] = 0;   // убираем \n
    
        Table *table = database_get_table(db, table_name);

        if (table == NULL) 
        {
            char response[4];
            int c;
            while ((c = getchar()) != '\n' && c != EOF);

            printf("Table not found. Create it? (y/n): ");
            
            fgets(response, sizeof(response), stdin);
            size_t len = strlen(response);
            if (len > 0 && response[len - 1] == '\n') {
                response[len - 1] = '\0';
            }
            if (response[0] == 'y' || response[0] == 'Y') 
            {
                database_create_table(db, table_name);
                Table *new_table = database_get_table(db, table_name);
                db->current_table = new_table;
                printf("Table '%s' created and selected.\n", table_name);
                return META_COMMAND_SUCCESS;
            }
            else 
            {
                printf("Table not created.\n");
                return META_COMMAND_UNRECOGNIZED_COMMAND;
            }
        }   
    
        db->current_table = table;
        printf("Using table '%s'\n", table_name);
        return META_COMMAND_SUCCESS;
    }

    else if (strcmp(input_buffer->buffer, ".exit") == 0 || strcmp(input_buffer->buffer, ".quit") == 0)  
    {
        database_close(db);
        exit(EXIT_SUCCESS);
    }  
    else if (strcmp(input_buffer->buffer, ".constants") == 0) 
    {
        printf("Constants:\n");
        print_constants();
        return META_COMMAND_SUCCESS;
    }

    else if (strcmp(input_buffer->buffer, ".btree") == 0) 
    {
        printf("Tree num:%d\n", db->schema.table_count);

        for (uint8_t i = 0; i < db->schema.table_count; i++)
        {
            printf("Tree Number %d\n", i);
            print_tree(db->pager, db->schema.tables[i].root_page_num, 0);
        }
        return META_COMMAND_SUCCESS;
    }
    else 
    {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}