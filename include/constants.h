// include/constants.h
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define COLUMN_USERNAME_SIZE 32 
#define COLUMN_EMAIL_SIZE 255 
#define PAGE_SIZE 4096  // Paging: Разделяем всё хранилище на блоки фиксированного размера (страницы), например 4096 байт.
#define TABLE_MAX_PAGES 100 //pages[] — массив указателей на эти блоки. Загружаем их по мере необходимости.


#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)



#endif
