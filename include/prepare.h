#ifndef PREPARE_H
#define PREPARE_H

#include "input_buffer.h"
#include "statement.h"

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement);

#endif
