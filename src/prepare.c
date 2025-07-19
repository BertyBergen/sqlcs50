#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/prepare.h"

PrepareResult prepare_insert(InputBuffer *input_buffer, Statement *statement) 
{ 
    statement->type = STATEMENT_INSERT;

    char* keyword = strtok(input_buffer->buffer, " ");
    char* id_string = strtok(NULL, " ");
    char* username = strtok(NULL, " ");
    char* email = strtok(NULL, " ");

    if (id_string == NULL || username == NULL || email == NULL) 
    {
        return PREPARE_SYNTAX_ERROR;
    }

    int id = atoi(id_string);
    if (id < 0)
    {
        return PREPARE_NEGATIVE_ID;
    }
    if (strlen(username) > COLUMN_USERNAME_SIZE) 
    {
        return PREPARE_STRING_TOO_LONG;
    }
    if (strlen(email) > COLUMN_EMAIL_SIZE) 
    {
        return PREPARE_STRING_TOO_LONG;
    }

    statement->row_to_insert.id = id;
    strcpy(statement->row_to_insert.username, username);
    strcpy(statement->row_to_insert.email, email);
    statement->row_to_insert.is_deleted = 0;

    return PREPARE_SUCCESS;
}

PrepareResult prepare_delete(InputBuffer *input_buffer, Statement *statement) 
{
    statement->type = STATEMENT_DELETE;
    char* keyword = strtok(input_buffer->buffer, " ");
    char* id_string = strtok(NULL, " ");
    
    if (id_string == NULL) 
    {
        return PREPARE_SYNTAX_ERROR;
    }
    int id = atoi(id_string);
    if (id < 0)
    {
        return PREPARE_NEGATIVE_ID;
    }
    statement->id_to_delete = id;

    return PREPARE_SUCCESS;

}

PrepareResult prepare_update(InputBuffer *input_buffer, Statement *statement) 
{ 
    statement->type = STATEMENT_UPDATE;

    char* keyword = strtok(input_buffer->buffer, " ");
    char* id_string = strtok(NULL, " ");
    char* username = strtok(NULL, " ");
    char* email = strtok(NULL, " ");

    if (id_string == NULL || username == NULL || email == NULL) 
    {
        return PREPARE_SYNTAX_ERROR;
    }

    int id = atoi(id_string);
    if (id < 0)
    {
        return PREPARE_NEGATIVE_ID;
    }
    
    if (strlen(username) > COLUMN_USERNAME_SIZE) 
    {
        return PREPARE_STRING_TOO_LONG;
    }
    if (strlen(email) > COLUMN_EMAIL_SIZE) 
    {
        return PREPARE_STRING_TOO_LONG;
    }

    statement->row_to_insert.id = id;
    strcpy(statement->row_to_insert.username, username);
    strcpy(statement->row_to_insert.email, email);

    return PREPARE_SUCCESS;
}

PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *statement) 
{
    char *lower = to_lowercase_cpy(input_buffer->buffer);
    if (!lower) return PREPARE_UNRECOGNIZED_STATEMENT;

    if (strncmp(lower, "insert", 6) == 0) 
    {
        return prepare_insert(input_buffer, statement);
    }
    if (strncmp(lower, "select", 6) == 0) 
    {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    if (strncmp(lower, "delete", 6) == 0)
    {
        return prepare_delete(input_buffer, statement);
    }
    if (strncmp(lower, "update", 6) == 0)
    {
        return prepare_update(input_buffer, statement);
    }
    
    free(lower);
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

char *to_lowercase_cpy(char *str)
{
    size_t len = strlen(str);
    char *copy = malloc(len + 1);
    
    if (!copy) return NULL;

    for (size_t i = 0; str[i]; i++)
    {
        copy[i] = tolower(str[i]);
    }
    copy[len] = '\0';
    return copy;
}
