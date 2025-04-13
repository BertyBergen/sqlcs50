#include "../include/table.h"
#include "../include/cursor.h"
#include <fcntl.h> 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h>  

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);

const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES; // What pages is?  


Table* db_open(const char *filename)
{
    Pager *pager  = pager_open(filename);
    uint32_t num_rows = pager->file_length / ROW_SIZE;

    Table *table = malloc(sizeof(Table));
    table->pager = pager;
    table->num_rows = num_rows;

    return table;
}

Pager *pager_open(const char *filename) // open(filename, flags, mode) возвращает дескриптор файла
{
    int fd = open(filename,
                    O_RDWR |        //Read/Write mode flag
                        O_CREAT,    // Create file if it doesn't exist flag
                    S_IWUSR |       // User write permission mode
                        S_IRUSR      // User read permission mode 
                );
    if (fd == -1)
    {
        printf("Unable to open file\n");
        exit(EXIT_FAILURE);
    }
    off_t file_length = lseek(fd, 0, SEEK_END); // lseek(fd дескриптор файла, offset смещение , whence откуда отмеряем)  Перемещает указатель чтения/записи в файле. В данном случае от нуля до конца. Получаем результат размер файла в байтах. Тип off_t просто число типа long

    Pager *pager = malloc(sizeof(Pager)); 
    pager->file_descriptor = fd; // Дескриптор — это просто целое число, которое операционная система использует для идентификации открытых ресурсов, таких как файлы, сокеты, каналы и т. д
    pager->file_length = file_length;
    
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
        pager->pages[i] = NULL;
    }
    return pager;
}


void *get_page(Pager *pager, uint32_t page_num)
{
    if (page_num > TABLE_MAX_PAGES)
    {
        printf("Tried to fetch page number out of bounds. %d > %d\n", page_num, TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    if (pager->pages[page_num] == NULL)
    {
        //Cache miss. Allocate memory and load from file.
        void *page = malloc(PAGE_SIZE);
        uint32_t num_page = pager->file_length / PAGE_SIZE;

        // WE might save a partial page at the end of the file
        if (pager->file_length % PAGE_SIZE)
        {
            num_page += 1;
        }
        if (page_num <= num_page)
        {
            lseek(pager->file_descriptor, page_num*PAGE_SIZE, SEEK_SET);            
            ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
            if (bytes_read == -1) 
            {
                printf("Error reading file: %d\n", errno);
                exit(EXIT_FAILURE);
            }
        }
        pager->pages[page_num] = page;
    }
    return pager->pages[page_num];
}

void serialize_row(Row* source, void* destination) 
{
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
    //strncpy(destination + USERNAME_OFFSET, source->username, USERNAME_SIZE);
   //strncpy(destination + EMAIL_OFFSET, source->email, EMAIL_SIZE);
}

void deserialize_row(void *source, Row* destination) 
{
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}
// Binary Serialization(Сериализация): Переводим структуру Row в "сырые" байты (memcpy) перед сохранением в память/файл. 
// Позволяет просто и быстро сохранять данные и читать обратно.

void db_close(Table* table) 
{
    Pager* pager = table->pager;
    uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE; // ERRRORR

    for (uint32_t i = 0; i < num_full_pages; i++) {
        if (pager->pages[i] == NULL) {
        continue;
        }
        pager_flush(pager, i, PAGE_SIZE);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    // There may be a partial page to write to the end of the file
    // This should not be needed after we switch to a B-tree
    uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE; // Это нужно для того, чтобы последняя старница, к примеру, неполностью заполненная, могла так же обработаться. ТО есть мы записываем ровно столько, сколько нужно, а не весь кластер. Разумное использование памяти. 
    if (num_additional_rows > 0) {
        uint32_t page_num = num_full_pages;
        if (pager->pages[page_num] != NULL) {
        pager_flush(pager, page_num, num_additional_rows * ROW_SIZE);
        free(pager->pages[page_num]);
        pager->pages[page_num] = NULL; // Освобождаем значения, которые присвоены указателю какому-либо
        }
    }

    int result = close(pager->file_descriptor);
    if (result == -1) {
        printf("Error closing db file.\n");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) { // Освобождаем указатель, нужно делать всегда
        void* page = pager->pages[i];
        if (page) {
        free(page);
        pager->pages[i] = NULL;
        }
    }

    free(pager);
    free(table);
}
void pager_flush(Pager* pager, uint32_t page_num, uint32_t size)
{
    if (pager->pages[page_num] == NULL) {
      printf("Tried to flush null page\n");
      exit(EXIT_FAILURE);
    }
  
    off_t offset = lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
  
    if (offset == -1) {
      printf("Error seeking: %d\n", errno);
      exit(EXIT_FAILURE);
    }
  
    ssize_t bytes_written =
        write(pager->file_descriptor, pager->pages[page_num], size);
  
    if (bytes_written == -1) {
      printf("Error writing: %d\n", errno);
      exit(EXIT_FAILURE);
    }
}
  
// void free_table(Table* table) {
//     //Цикл прерывается, если table->pages[i] == NULL. Но это не значит, что дальше страниц нет — просто одна из них может не использоваться.Если, скажем, у тебя были страницы в pages[0], pages[2], а pages[1] не была аллоцирована (NULL), то pages[2] не будет освобождена.
//     for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
//         free(table->pages[i]);
//     }
//     free(table);
// }
