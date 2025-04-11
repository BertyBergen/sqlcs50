#ifndef STATEMENT_H
#define STATEMENT_H

#include "table.h"

// Структура и типы для SQL-операторов
typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef struct {
    StatementType type;
    Row row_to_insert; // только для оператора вставки
} Statement;

#endif
