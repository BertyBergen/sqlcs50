#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/meta_commands.h"

MetaCommandResult do_meta_command(InputBuffer* input_buffer, Table *table)
{
    if (strcmp(input_buffer->buffer, ".exit") == 0 || strcmp(input_buffer->buffer, ".quit") == 0)  {
        close_input_buffer(input_buffer);
        free_table(table);
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}