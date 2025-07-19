#ifndef PREPARE_H
#define PREPARE_H

#include "input_buffer.h"
#include "execute.h"


typedef enum {
    PREPARE_SUCCESS,
    PREPARE_STRING_TOO_LONG,
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_NEGATIVE_ID
} PrepareResult;


PrepareResult prepare_insert(InputBuffer *input_buffer, Statement *statement);
PrepareResult prepare_delete(InputBuffer *input_buffer, Statement *statement);
PrepareResult prepare_update(InputBuffer *input_buffer, Statement *statement);
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement);
char *to_lowercase_cpy(char *str);

#endif
