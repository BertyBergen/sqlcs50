#ifndef META_COMMANDS_H
#define META_COMMANDS_H

#include "table.h"
#include "input_buffer.h"


typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef struct {
    const char *name;
    const char *description;
} MetaCommand;


MetaCommandResult do_meta_command(InputBuffer* input_buffer, Database *db);

#endif