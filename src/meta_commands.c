#include <stdlib.h>
#include "../include/meta_commands.h"
#include <string.h>

MetaCommandResult do_meta_command(InputBuffer* input_buffer, Table* table) 
{
    if (strcmp(input_buffer->buffer, ".exit") == 0 || strcmp(input_buffer->buffer, ".quit") == 0)  {
        db_close(table);
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}