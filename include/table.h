#ifndef TABLE_H
#define TABLE_H

#include <fcntl.h> 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h>  

#define COLUMN_USERNAME_SIZE 32 
#define COLUMN_EMAIL_SIZE 255 
#define PAGE_SIZE 4096  // Paging: Разделяем всё хранилище на блоки фиксированного размера (страницы), например 4096 байт.
#define TABLE_MAX_PAGES 100 //pages[] — массив указателей на эти блоки. Загружаем их по мере необходимости.Помогает управлять памятью и избежать чтения всего файла целиком.

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

typedef struct {
    int file_descriptor;
    uint32_t file_length;
    void *pages[TABLE_MAX_PAGES];
} Pager;

typedef struct {
    Pager *pager;
    uint32_t num_rows;
} Table;

typedef struct {// Layout  Каждая строка (Row) занимает строго фиксированный размер: id (4 байта), username (32), email (255).
    uint32_t id; // Используем оффсеты для записи/чтения: ID_OFFSET, USERNAME_OFFSET и т.д.
    char username[COLUMN_USERNAME_SIZE + 1]; // Такой layout нужен для бинарной сериализации.
    char email[COLUMN_EMAIL_SIZE + 1];
} Row;

extern const uint32_t ID_SIZE;
extern const uint32_t USERNAME_SIZE;
extern const uint32_t EMAIL_SIZE;

extern const uint32_t ID_OFFSET;
extern const uint32_t USERNAME_OFFSET;
extern const uint32_t EMAIL_OFFSET;
extern const uint32_t ROW_SIZE;

extern const uint32_t ROWS_PER_PAGE;
extern const uint32_t TABLE_MAX_ROWS;

Table* db_open(const char *filename);
Pager *pager_open(const char *filename);
void *get_page(Pager *pager, uint32_t page_num);
void pager_flush(Pager* pager, uint32_t page_num, uint32_t size);
void serialize_row(Row* source, void* destination);
void deserialize_row(void *source, Row* destination);
void* row_slot(Table* table, uint32_t row_num);
void print_row(Row* row);
void db_close(Table* table) ;

#endif
