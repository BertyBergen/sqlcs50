#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cursor.h"
#include "../include/btree.h"
// #include "../include/table.h"

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

Cursor *leaf_node_find(Table *table, uint32_t page_num, uint32_t key) 
//Это либо вернет положение ключа, положение другого ключа, который нам нужно будет переместить, 
//если мы захотим вставить новый ключ, или позиция, следующая за последней клавишей
{
    void *node = get_page(table->pager, page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);
  
    Cursor *cursor = malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->page_num = page_num;
    cursor->end_of_table = false;
  
    // Binary search
    uint32_t min_index = 0;
    uint32_t one_past_max_index = num_cells;
    while (one_past_max_index != min_index) 
    {
      uint32_t index = (min_index + one_past_max_index) / 2;
      uint32_t key_at_index = *leaf_node_key(node, index);
      
      if (key == key_at_index) 
      {
        cursor->cell_num = index;
        return cursor;
      }
      if (key < key_at_index) 
      {
        one_past_max_index = index;
      } 
      else 
      {
        min_index = index + 1;
      }
    }
  
    cursor->cell_num = min_index;
    return cursor;
}

Cursor *table_find(Table *table, uint32_t key) 
{
    uint32_t root_page_num = table->root_page_num;
    void *root_node = get_page(table->pager, root_page_num);

    if (get_node_type(root_node) == NODE_LEAF) 
    {
        return leaf_node_find(table, root_page_num, key);
    } 
    else 
    {
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
        } 
        else 
        {
          min_index = index + 1;
        }
    }
    return min_index;
}

Cursor *internal_node_find(Table* table, uint32_t page_num, uint32_t key) 
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



void internal_node_insert(Table *table, uint32_t parent_page_num, uint32_t child_page_num) 
{
    // Add a new child/key pair to parent that corresponds to child
    
    void* parent = get_page(table->pager, parent_page_num);
    void* child = get_page(table->pager, child_page_num);
    uint32_t child_max_key = get_node_max_key(table->pager, child);
    uint32_t index = internal_node_find_child(parent, child_max_key);

    uint32_t original_num_keys = *internal_node_num_keys(parent);

    if (original_num_keys >= INTERNAL_NODE_MAX_KEYS) 
    {
        internal_node_split_and_insert(table, parent_page_num, child_page_num);
        return;
    }

    uint32_t right_child_page_num = *internal_node_right_child(parent);
    // An internal node with a right child of INVALID_PAGE_NUM is empty
    if (right_child_page_num == INVALID_PAGE_NUM) 
    {
        *internal_node_right_child(parent) = child_page_num;
        return;
    }

    void* right_child = get_page(table->pager, right_child_page_num);
    /*
    If we are already at the max number of cells for a node, we cannot increment
    before splitting. Incrementing without inserting a new key/child pair
    and immediately calling internal_node_split_and_insert has the effect
    of creating a new key at (max_cells + 1) with an uninitialized value
    */
   
    *internal_node_num_keys(parent) = original_num_keys + 1;

    if (child_max_key > get_node_max_key(table->pager, right_child)) 
    {
        /* Replace right child */
        *internal_node_child(parent, original_num_keys) = right_child_page_num;
        *internal_node_key(parent, original_num_keys) = get_node_max_key(table->pager, right_child);
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

void internal_node_split_and_insert(Table* table, uint32_t parent_page_num, uint32_t child_page_num) 
{
    uint32_t old_page_num = parent_page_num;
    void* old_node = get_page(table->pager, parent_page_num);
    uint32_t old_max = get_node_max_key(table->pager, old_node);

    void* child = get_page(table->pager, child_page_num); 
    uint32_t child_max = get_node_max_key(table->pager, child);

    uint32_t new_page_num = get_unused_page_num(table->pager);
    
    /*
    Declaring a flag before updating pointers which
    records whether this operation involves splitting the root -
    if it does, we will insert our newly created node during
    the step where the table's new root is created. If it does
    not, we have to insert the newly created node into its parent
    after the old node's keys have been transferred over. We are not
    able to do this if the newly created node's parent is not a newly
    initialized root node, because in that case its parent may have existing
    keys aside from our old node which we are splitting. If that is true, we
    need to find a place for our newly created node in its parent, and we
    cannot insert it at the correct index if it does not yet have any keys
    */

    uint32_t splitting_root = is_node_root(old_node);

    void* parent;
    void* new_node;

    if (splitting_root) {
        // Если мы сплитим корень, создаём новый корень
        create_new_root(table, new_page_num);
        parent = get_page(table->pager, table->root_page_num);
      
        // После создания нового корня, обновляем old_node (он теперь левый ребенок нового корня)
        /*
        If we are splitting the root, we need to update old_node to point
        to the new root's left child, new_page_num will already point to
        the new root's right child
        */
        old_page_num = *internal_node_child(parent, 0);
        old_node = get_page(table->pager, old_page_num);
    } 
    else 
    {
        parent = get_page(table->pager, *node_parent(old_node));
        new_node = get_page(table->pager, new_page_num);
        initialize_internal_node(new_node);
    }

    uint32_t* old_num_keys = internal_node_num_keys(old_node);

    // Получаем текущего правого ребенка old_node
    uint32_t cur_page_num = *internal_node_right_child(old_node);
    void* cur = get_page(table->pager, cur_page_num);

    // Переносим правого ребенка в новый узел
    internal_node_insert(table, new_page_num, cur_page_num);
    *node_parent(cur) = new_page_num;

    // Очищаем правого ребенка у старого узла
    *internal_node_right_child(old_node) = INVALID_PAGE_NUM;

    // Переносим половину ключей/указателей в новый узел
    for (int i = INTERNAL_NODE_MAX_KEYS - 1; i > INTERNAL_NODE_MAX_KEYS / 2; i--) {
        cur_page_num = *internal_node_child(old_node, i);
        cur = get_page(table->pager, cur_page_num);

        internal_node_insert(table, new_page_num, cur_page_num);
        *node_parent(cur) = new_page_num;

        (*old_num_keys)--;
    }

    // Последний ключ (до середины) становится новым правым ребёнком
    *internal_node_right_child(old_node) = *internal_node_child(old_node, *old_num_keys - 1);
    (*old_num_keys)--;

    // Определяем, куда нужно вставить новый дочерний узел: в старый или в новый
    uint32_t max_after_split = get_node_max_key(table->pager, old_node);
    uint32_t destination_page_num = (child_max < max_after_split) ? old_page_num : new_page_num;

    // Вставляем новый дочерний узел
    internal_node_insert(table, destination_page_num, child_page_num);
    *node_parent(child) = destination_page_num;

    // Обновляем ключ в родителе: old_max → новый максимум в old_node
    update_internal_node_key(parent, old_max, get_node_max_key(table->pager, old_node));

    if (!splitting_root) {
        // Вставляем ссылку на новый узел в родителя
        internal_node_insert(table, *node_parent(old_node), new_page_num);
        *node_parent(new_node) = *node_parent(old_node);
    }
}
