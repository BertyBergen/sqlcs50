// include/constants.h
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255 
#define PAGE_SIZE 4096  // Paging: Разделяем всё хранилище на блоки фиксированного размера (страницы), например 4096 байт.
#define MAX_PAGES 32000 //pages[] — массив указателей на эти блоки. Загружаем их по мере необходимости.

#define MAX_TABLE_NAME 32
#define MAX_TABLES 100
#define SCHEMA_PAGE 0


#define INVALID_PAGE_NUM UINT32_MAX

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)



#endif
