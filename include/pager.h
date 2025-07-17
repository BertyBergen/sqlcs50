#ifndef PAGER_H
#define PAGER_H

#include <stdint.h>
#include "row.h" 

typedef struct {
    int file_descriptor;
    uint32_t file_length;
    uint32_t num_pages;
    uint32_t free_page_head;
    void *pages[MAX_PAGES];
} Pager;

Pager *pager_open(const char *filename);
void *get_page(Pager *pager, uint32_t page_num);
void pager_flush(Pager* pager, uint32_t page_num);
uint32_t get_unused_page_num(Pager* pager);
void print_tree(Pager *pager, uint32_t page_num, uint32_t indentation_level);
uint32_t get_node_max_key(Pager *pager,void *node);
uint32_t get_node_min_key(Pager* pager, void* node);
void set_page_free(Pager *pager, uint32_t page_num);


#endif
