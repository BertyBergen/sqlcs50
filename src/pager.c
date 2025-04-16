
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "../include/pager.h"


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
    
    pager->num_pages = (file_length / PAGE_SIZE);

    if (file_length % PAGE_SIZE != 0) {
        printf("Db file is not a whole number of pages. Corrupt file.\n");
        exit(EXIT_FAILURE);
    }

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
        if (page_num >= pager->num_pages) {
            pager->num_pages = page_num + 1;
        }
    }
    return pager->pages[page_num];
}

void pager_flush(Pager* pager, uint32_t page_num) 
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
        // write(pager->file_descriptor, pager->pages[page_num], size);
        write(pager->file_descriptor, pager->pages[page_num], PAGE_SIZE);
  
    if (bytes_written == -1) {
      printf("Error writing: %d\n", errno);
      exit(EXIT_FAILURE);
    }
}
