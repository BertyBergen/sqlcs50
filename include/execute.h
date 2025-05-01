#ifndef EXECUTE_H
#define EXECUTE_H

#include "btree.h"
#include "cursor.h"
#include "statement.h"

typedef enum 
{ 
    EXECUTE_SUCCESS,
    EXECUTE_DUPLICATE_KEY
} ExecuteResult;

ExecuteResult execute_insert(Statement* statement, Table* table);
ExecuteResult execute_select(Statement* statement, Table* table);
ExecuteResult execute_statement(Statement* statement, Table *table);

#endif
