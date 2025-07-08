#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "./include/input_buffer.h"
#include "./include/meta_commands.h"
#include "./include/prepare.h"
#include "./include/execute.h"

int main(int argc, char* argv[]) 
{
    if (argc < 2)
    {
        printf("Must supply a database flename. \n");
        exit(EXIT_FAILURE);
    }
    
    char *filename = argv[1];
    Database *db = database_open(filename);

    InputBuffer* input_buffer = new_input_buffer();
    
    printf("Welcome to sqlcs50!\n");
    printf("Type '.help' to see available commands.\n");
    printf("You can execute SQL statements ending with ';'.\n\n");

    while (true) 
    {
        read_input(input_buffer);
        
        if (input_buffer->buffer[0] == '.') 
        {
            switch (do_meta_command(input_buffer, db)) 
            {
                case META_COMMAND_SUCCESS:
                    continue;
                case META_COMMAND_UNRECOGNIZED_COMMAND:
                    printf("Unrecognized command '%s'\n", input_buffer->buffer);
                    continue;
            }
        }
        
        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) 
        {
            case PREPARE_SUCCESS:
                break;
            case PREPARE_STRING_TOO_LONG:
                printf("String is too long.\n");
                continue;
            case PREPARE_SYNTAX_ERROR:
                printf("Syntax error. Could not parse statement.\n");
                continue;
            case PREPARE_NEGATIVE_ID:
                printf("ID must be positive.\n");
                continue;
            case PREPARE_UNRECOGNIZED_STATEMENT:
                printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
                continue;
     
        }
        
        if (db->current_table == NULL) {
            printf("No table selected. Use '.use <table_name>'\n");
            continue;
        }

        switch (execute_statement(&statement, db)) 
        {
            case EXECUTE_SUCCESS:
                printf("Executed.\n");
                break;
            case (EXECUTE_DUPLICATE_KEY):
                printf("Error: Duplicate key.\n");
                break;
            case (EXECUTE_NO_ID):
                printf("Error: Id not found.\n");
                break;
        }
    }
}