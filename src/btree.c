#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/btree.h"
#include "../include/row.h"

uint32_t *leaf_node_num_cells(void *node) 
{
    return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

uint32_t *leaf_node_next_leaf(void *node) 
{
    return node + LEAF_NODE_NEXT_LEAF_OFFSET;
}

// Возвращает указатель на начало ячейки с номером cell_num
void *leaf_node_cell(void *node, uint32_t cell_num) 
{
    return node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

// Возвращает указатель на ключ в ячейке
uint32_t *leaf_node_key(void *node, uint32_t cell_num) 
{
    return leaf_node_cell(node, cell_num);
}

// Возвращает указатель на значение в ячейке
void *leaf_node_value(void *node, uint32_t cell_num)
{
    return leaf_node_cell(node, cell_num) + LEAF_NODE_KEY_SIZE;
}

NodeType get_node_type(void *node) 
{
    uint8_t value = *((uint8_t*)(node + NODE_TYPE_OFFSET));
    return (NodeType)value;
}
    
void set_node_type(void *node, NodeType type) 
{
    uint8_t value = type;
    *((uint8_t*)(node + NODE_TYPE_OFFSET)) = value;
}

bool is_node_root(void *node) 
{
    uint8_t value = *((uint8_t*)(node + IS_ROOT_OFFSET));
    return (bool)value;
}
    
void set_node_root(void *node, bool is_root) 
{
    uint8_t value = is_root;
    *((uint8_t*)(node + IS_ROOT_OFFSET)) = value;
}

uint32_t *node_parent(void *node) 
{ 
    return node + PARENT_POINTER_OFFSET; 
}
uint32_t *internal_node_num_keys(void *node) 
{
      return node + INTERNAL_NODE_NUM_KEYS_OFFSET;
}
    
uint32_t *internal_node_right_child(void* node) 
{
    return node + INTERNAL_NODE_RIGHT_CHILD_OFFSET;
}

uint32_t *internal_node_cell(void* node, uint32_t cell_num) 
{
    return node + INTERNAL_NODE_HEADER_SIZE + cell_num * INTERNAL_NODE_CELL_SIZE;
}

uint32_t *internal_node_child(void *node, uint32_t child_num) // FAIL
{
    uint32_t num_keys = *internal_node_num_keys(node);
    if (child_num > num_keys) 
    {
        printf("Tried to access child_num %d > num_keys %d\n", child_num, num_keys);
        exit(EXIT_FAILURE);
    } 
    else if (child_num == num_keys) 
    {
        uint32_t* right_child = internal_node_right_child(node);
        if (*right_child == INVALID_PAGE_NUM) 
        {
            printf("Tried to access right child of node, but was invalid page\n");
            exit(EXIT_FAILURE);
        }
        return right_child;
    }
    else 
    {
       
        uint32_t *child = internal_node_cell(node, child_num);
        if (*child == INVALID_PAGE_NUM) 
        {
            printf("Tried to access child %d of node, but was invalid page\n", child_num);
            exit(EXIT_FAILURE);
        }
    return child;
    }
}

uint32_t *internal_node_key(void *node, uint32_t key_num) 
{
    return internal_node_cell(node, key_num) + INTERNAL_NODE_CHILD_SIZE;
}

void update_internal_node_key(void *node, uint32_t old_key, uint32_t new_key) 
{
    uint32_t old_child_index = internal_node_find_child(node, old_key);
    // *internal_node_key(node, old_child_index) = new_key;
     // Добавляем проверку, чтобы убедиться, что ключ найден
     if (old_child_index < *internal_node_num_keys(node)) 
     {
        *internal_node_key(node, old_child_index) = new_key;
    }
    else 
    {
        // Если ключ не найден, можно выбросить ошибку или выполнить другую логику
        printf("Error: Old key not found in the internal node.\n");
    }
}



void initialize_leaf_node(void *node) 
{
    set_node_type(node, NODE_LEAF);
    set_node_root(node, false);
    *leaf_node_num_cells(node) = 0;
    *leaf_node_next_leaf(node) = 0;
}

// Вставка нового узла в упорядоченный массив внутри листа
void leaf_node_insert(Cursor *cursor, uint32_t key, Row *value) 
{
    void *node = get_page(cursor->table->pager, cursor->page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);
    if (num_cells >= LEAF_NODE_MAX_CELLS) 
    {
        // Node full
        leaf_node_split_and_insert(cursor, key, value);
        return;
    }

    if (cursor->cell_num < num_cells) 
    {
        // Make room for new cell
        for (uint32_t i = num_cells; i > cursor->cell_num; i--) 
        {
            memcpy(leaf_node_cell(node, i), leaf_node_cell(node, i - 1),
                    LEAF_NODE_CELL_SIZE);
        }
    }

    *(leaf_node_num_cells(node)) += 1;
    *(leaf_node_key(node, cursor->cell_num)) = key;
    serialize_row(value, leaf_node_value(node, cursor->cell_num));
}

void leaf_node_split_and_insert(Cursor *cursor, uint32_t key, Row *value) 
{
    /*
    Create a new node and move half the cells over.
    Insert the new value in one of the two nodes.
    Update parent or create a new parent.
    */
    void *old_node = get_page(cursor->table->pager, cursor->page_num);
    uint32_t old_max = get_node_max_key(cursor->table->pager, old_node);
    uint32_t new_page_num = get_unused_page_num(cursor->table->pager);

    void *new_node = get_page(cursor->table->pager, new_page_num);
    initialize_leaf_node(new_node);
    
    *node_parent(new_node) = *node_parent(old_node);
    *leaf_node_next_leaf(new_node) = *leaf_node_next_leaf(old_node);
    *leaf_node_next_leaf(old_node) = new_page_num;

    /*
    All existing keys plus new key should be divided
    evenly between old (left) and new (right) nodes.
    Starting from the right, move each key to correct position.
    */
    for (int32_t i = LEAF_NODE_MAX_CELLS; i >= 0; i--) 
    {
        void *destination_node;

        if (i >= LEAF_NODE_LEFT_SPLIT_COUNT) 
        {
            destination_node = new_node;
        } 
        else 
        {
            destination_node = old_node;
        }
        
        uint32_t index_within_node = i % LEAF_NODE_LEFT_SPLIT_COUNT;
        void *destination = leaf_node_cell(destination_node, index_within_node);
        
        if (i == cursor->cell_num) 
        {
            serialize_row(value, leaf_node_value(destination_node, index_within_node));
            *leaf_node_key(destination_node, index_within_node) = key;
        } 
        else if (i > cursor->cell_num) 
        {
            memcpy(destination, leaf_node_cell(old_node, i - 1), LEAF_NODE_CELL_SIZE);
        } 
        else 
        {
            memcpy(destination, leaf_node_cell(old_node, i), LEAF_NODE_CELL_SIZE);
        }
    }
    /* Update cell count on both leaf nodes */
    *(leaf_node_num_cells(old_node)) = LEAF_NODE_LEFT_SPLIT_COUNT;
    *(leaf_node_num_cells(new_node)) = LEAF_NODE_RIGHT_SPLIT_COUNT;
    
    if (is_node_root(old_node)) 
    {
        return create_new_root(cursor->table, new_page_num);
    } 
    else 
    {
        uint32_t parent_page_num = *node_parent(old_node);
        uint32_t new_max = get_node_max_key(cursor->table->pager, old_node);
        void* parent = get_page(cursor->table->pager, parent_page_num);
    
        update_internal_node_key(parent, old_max, new_max);
        internal_node_insert(cursor->table, parent_page_num, new_page_num);
        return;
    }
}

void create_new_root(struct Table *table, uint32_t right_child_page_num) 
{
    /*
    Handle splitting the root.
    Old root copied to new page, becomes left child.
    Address of right child passed in.
    Re-initialize root page to contain the new root node.
    New root node points to two children.
    */
    void* root = get_page(table->pager, table->root_page_num); //Получаем указатель на текущий корневой узел.
    void* right_child = get_page(table->pager, right_child_page_num); // Получаем указатель на правого ребёнка, который получился в результате сплита.
    uint32_t left_child_page_num = get_unused_page_num(table->pager);
    // Выделяем новую страницу для левого ребёнка, куда мы скопируем старый корень.
    void* left_child = get_page(table->pager, left_child_page_num); 

    if (get_node_type(root) == NODE_INTERNAL) 
    {
        initialize_internal_node(right_child);
        initialize_internal_node(left_child);
    }
    /* Left child has data copied from old root */
    memcpy(left_child, root, PAGE_SIZE); // Копируем старый корень  в левого ребенка.
    set_node_root(left_child, false); // Отмечаем, что он больше не корень.
    
    if (get_node_type(left_child) == NODE_INTERNAL) 
    {
        void *child;
        for (int i = 0; i < *internal_node_num_keys(left_child); i++) 
        {
            child = get_page(table->pager, *internal_node_child(left_child,i));
            *node_parent(child) = left_child_page_num;
        }
        child = get_page(table->pager, *internal_node_right_child(left_child));
        *node_parent(child) = left_child_page_num;
    }

    /* Root node is a new internal node with one key and two children */
    initialize_internal_node(root); // Инициализируем текущую страницу root как новый внутренний узел (internal node).
    set_node_root(root, true); // Обозначаем его как новый корень.
    *internal_node_num_keys(root) = 1; // У нового корня только один ключ.
    *internal_node_child(root, 0) = left_child_page_num; // Первый ребёнок нового корня — это левый узел (старый корень).
    uint32_t left_child_max_key  = get_node_max_key(table->pager, left_child); // Извлекаем минимальный ключ из правого узла и записываем его в корень.
    *internal_node_key(root, 0) = left_child_max_key; // Это ключ, который помогает навигации: всё, что ≤ этому значению — идёт в левое поддерево.
    *internal_node_right_child(root) = right_child_page_num; // Устанавливаем правого ребёнка корня — это тот узел, что получился из сплита.
    *node_parent(left_child) = table->root_page_num;
    *node_parent(right_child) = table->root_page_num;
}

void initialize_internal_node(void *node) 
{
    set_node_type(node, NODE_INTERNAL);
    set_node_root(node, false);
    *internal_node_num_keys(node) = 0;
    /*
    Necessary because the root page number is 0; by not initializing an internal 
    node's right child to an invalid page number when initializing the node, we may
    end up with 0 as the node's right child, which makes the node a parent of the root
    */
    *internal_node_right_child(node) = INVALID_PAGE_NUM;
}
