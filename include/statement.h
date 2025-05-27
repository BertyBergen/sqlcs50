#ifndef STATEMENT_H
#define STATEMENT_H

#include "table.h"
#include <stdint.h>
// Структура и типы для SQL-операторов
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

#endif
