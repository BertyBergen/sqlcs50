#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cursor.h"
#include "../include/table.h"

Cursor* table_start(Table* table) 
{
    Cursor* cursor =  table_find(table, 0);
  
    void* node = get_page(table->pager, cursor->page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);
    cursor->end_of_table = (num_cells == 0);
  
    return cursor;
}
// Return the position of the given key.
// If the key is not present, return the position
// where it should be inserted

Cursor* leaf_node_find(Table* table, uint32_t page_num, uint32_t key) //Это либо вернет положение ключа, положение другого ключа, который нам нужно будет переместить, если мы захотим вставить новый ключ, или позиция, следующая за последней клавишей
{
    void* node = get_page(table->pager, page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);
  
    Cursor* cursor = malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->page_num = page_num;
  
    // Binary search
    uint32_t min_index = 0;
    uint32_t one_past_max_index = num_cells;
    while (one_past_max_index != min_index) {
      uint32_t index = (min_index + one_past_max_index) / 2;
      uint32_t key_at_index = *leaf_node_key(node, index);
      if (key == key_at_index) {
        cursor->cell_num = index;
        return cursor;
      }
      if (key < key_at_index) {
        one_past_max_index = index;
      } else {
        min_index = index + 1;
      }
    }
  
    cursor->cell_num = min_index;
    return cursor;
}

Cursor* table_find(Table* table, uint32_t key) 
{
    uint32_t root_page_num = table->root_page_num;
    void* root_node = get_page(table->pager, root_page_num);
  
    if (get_node_type(root_node) == NODE_LEAF) {
      return leaf_node_find(table, root_page_num, key);
    } else {
      return internal_node_find(table, root_page_num, key);
    }
}

void* cursor_value(Cursor *cursor)
{
    uint32_t page_num = cursor->page_num;
    void* page = get_page(cursor->table->pager, page_num);
    return leaf_node_value(page, cursor->cell_num);
}

void cursor_advance(Cursor *cursor) 
{
    uint32_t page_num = cursor->page_num;
    void *node = get_page(cursor->table->pager, page_num);

    cursor->cell_num += 1;
    if (cursor->cell_num >= (*leaf_node_num_cells(node))) 
    {
        /* Advance to next leaf node */
        uint32_t next_page_num = *leaf_node_next_leaf(node);
        if (next_page_num == 0) 
        {
          /* This was rightmost leaf */
          cursor->end_of_table = true;
        } else 
        {
          cursor->page_num = next_page_num;
          cursor->cell_num = 0;
        }
    }
}

uint32_t internal_node_find_child(void *node, uint32_t key)
{
    // Return the index of the child which should contain the given key.
    uint32_t num_keys = *internal_node_num_keys(node);

    // Binary search
    uint32_t min_index = 0;
    uint32_t max_index = num_keys; /* there is one more child than key */
    //Делает бинарный поиск по ключам, чтобы найти, в какого ребёнка спуститься.
    while (min_index != max_index) 
    {
        uint32_t index = (min_index + max_index) / 2;
        uint32_t key_to_right = *internal_node_key(node, index);
        if (key_to_right >= key) 
        {
          max_index = index;
        } else 
        {
          min_index = index + 1;
        }
    }
    uint32_t child_num = *internal_node_child(node, min_index);
    return min_index;
}

Cursor* internal_node_find(Table* table, uint32_t page_num, uint32_t key) 
{
    void* node = get_page(table->pager, page_num);
  
    uint32_t child_index = internal_node_find_child(node, key);
    uint32_t child_num = *internal_node_child(node, child_index);
    void* child = get_page(table->pager, child_num);
    switch (get_node_type(child)) 
    {
        case NODE_LEAF:
            return leaf_node_find(table, child_num, key);
        case NODE_INTERNAL:
            return internal_node_find(table, child_num, key);
    }
}


void internal_node_insert(Table* table, uint32_t parent_page_num, uint32_t child_page_num) 
{
    // Add a new child/key pair to parent that corresponds to child
    
    void* parent = get_page(table->pager, parent_page_num);
    void* child = get_page(table->pager, child_page_num);
    uint32_t child_max_key = get_node_max_key(table->pager, child);
    uint32_t index = internal_node_find_child(parent, child_max_key);

    uint32_t original_num_keys = *internal_node_num_keys(parent);

    if (original_num_keys >= INTERNAL_NODE_MAX_KEYS) 
    {
        printf("Need to implement splitting internal node\n");
        exit(EXIT_FAILURE);
        // internal_node_split_and_insert(table, parent_page_num, child_page_num);
        // return;
    }

    uint32_t right_child_page_num = *internal_node_right_child(parent);
    // An internal node with a right child of INVALID_PAGE_NUM is empty
    // if (right_child_page_num == INVALID_PAGE_NUM) 
    // {
    //     *internal_node_right_child(parent) = child_page_num;
    //     return;
    // }

    void* right_child = get_page(table->pager, right_child_page_num);
    /*
    If we are already at the max number of cells for a node, we cannot increment
    before splitting. Incrementing without inserting a new key/child pair
    and immediately calling internal_node_split_and_insert has the effect
    of creating a new key at (max_cells + 1) with an uninitialized value
    */
    // *internal_node_num_keys(parent) = original_num_keys + 1;

    if (child_max_key > get_node_max_key(table->pager, right_child)) 
    {
        /* Replace right child */
        *internal_node_child(parent, original_num_keys) = right_child_page_num;
        *internal_node_key(parent, original_num_keys) =
        get_node_max_key(table->pager, right_child);
        *internal_node_right_child(parent) = child_page_num;
    } 
    else 
    {
    /* Make room for the new cell */
    for (uint32_t i = original_num_keys; i > index; i--) 
    {
        void* destination = internal_node_cell(parent, i);
        void* source = internal_node_cell(parent, i - 1);
        memcpy(destination, source, INTERNAL_NODE_CELL_SIZE);
    }

    *internal_node_child(parent, index) = child_page_num;
    *internal_node_key(parent, index) = child_max_key;
    
  }
}

