#ifndef EXECUTE_H
#define EXECUTE_H

#include "btree.h"
#include "cursor.h"

typedef enum 
{ 
    EXECUTE_SUCCESS,
    EXECUTE_DUPLICATE_KEY,
    EXECUTE_NO_ID
} ExecuteResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT,
    STATEMENT_DELETE,
    STATEMENT_UPDATE,
    EXECUTE_FAILURE
} StatementType;

typedef struct {
    StatementType type;
    Row row_to_insert; // only used by insert statement
    uint32_t id_to_delete;
} Statement;


ExecuteResult execute_insert(Statement* statement, Table* table);
ExecuteResult execute_select(Database* db);
ExecuteResult execute_statement(Statement* statement, Database *db);

#endif
