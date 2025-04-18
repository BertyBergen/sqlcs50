#ifndef EXECUTE_H
#define EXECUTE_H

#include "table.h"
#include "row.h"
// #include "cursor.h"
#include "statement.h"

typedef enum 
{ 
    EXECUTE_SUCCESS,
    EXECUTE_DUPLICATE_KEY,
    EXECUTE_TABLE_FULL
} ExecuteResult;

ExecuteResult execute_insert(Statement* statement, Table* table);
ExecuteResult execute_select(Statement* statement, Table* table);
ExecuteResult execute_statement(Statement* statement, Table *table);

#endif
