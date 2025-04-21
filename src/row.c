#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/row.h"
#include "../include/table.h"

// struct Table;

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES; // What pages is? It's a cluster of memory cells  

//Common Node Header Layout
const uint32_t NODE_TYPE_SIZE = sizeof(uint8_t); 
const uint32_t NODE_TYPE_OFFSET = 0; 
const uint32_t IS_ROOT_SIZE = sizeof(uint8_t); // 
const uint32_t IS_ROOT_OFFSET = NODE_TYPE_SIZE; 
const uint32_t PARENT_POINTER_SIZE = sizeof(uint32_t); // Это не настоящий указатель, а индекс страницы, где лежит родитель. 
const uint32_t PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE; 
const uint8_t COMMON_NODE_HEADER_SIZE = NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE; 
    
// Leaf Node Header Layout
const uint32_t LEAF_NODE_NUM_CELLS_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_NEXT_LEAF_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_NEXT_LEAF_OFFSET =
    LEAF_NODE_NUM_CELLS_OFFSET + LEAF_NODE_NUM_CELLS_SIZE;
const uint32_t LEAF_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE +
                                       LEAF_NODE_NUM_CELLS_SIZE +
                                       LEAF_NODE_NEXT_LEAF_SIZE;
 
//Leaf Node Body Layout
const uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_KEY_OFFSET = 0;
const uint32_t LEAF_NODE_VALUE_SIZE = ROW_SIZE;
const uint32_t LEAF_NODE_VALUE_OFFSET = LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;
const uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
const uint32_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_MAX_CELLS = LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;

const uint32_t LEAF_NODE_RIGHT_SPLIT_COUNT = (LEAF_NODE_MAX_CELLS + 1) / 2;
const uint32_t LEAF_NODE_LEFT_SPLIT_COUNT =
    (LEAF_NODE_MAX_CELLS + 1) - LEAF_NODE_RIGHT_SPLIT_COUNT;


/*
 * Internal Node Header Layout
 */
const uint32_t INTERNAL_NODE_NUM_KEYS_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_NUM_KEYS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t INTERNAL_NODE_RIGHT_CHILD_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_RIGHT_CHILD_OFFSET =
    INTERNAL_NODE_NUM_KEYS_OFFSET + INTERNAL_NODE_NUM_KEYS_SIZE;
const uint32_t INTERNAL_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE +
                                           INTERNAL_NODE_NUM_KEYS_SIZE +
                                           INTERNAL_NODE_RIGHT_CHILD_SIZE;
/*
* Internal Node Body Layout
*/
const uint32_t INTERNAL_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_CHILD_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_CELL_SIZE =
    INTERNAL_NODE_CHILD_SIZE + INTERNAL_NODE_KEY_SIZE;
const uint32_t INTERNAL_NODE_MAX_KEYS = 3;

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

// Возвращает указатель на количество ячеек в листовом узле
uint32_t* leaf_node_num_cells(void* node) 
{
    return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

uint32_t* leaf_node_next_leaf(void* node) 
{
    return node + LEAF_NODE_NEXT_LEAF_OFFSET;
}

// Возвращает указатель на начало ячейки с номером cell_num
void* leaf_node_cell(void* node, uint32_t cell_num) 
{
    return node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

// Возвращает указатель на ключ в ячейке
uint32_t* leaf_node_key(void* node, uint32_t cell_num) 
{
    return leaf_node_cell(node, cell_num);
}

// Возвращает указатель на значение в ячейке
void* leaf_node_value(void* node, uint32_t cell_num)
{
    return leaf_node_cell(node, cell_num) + LEAF_NODE_KEY_SIZE;
}

// Инициализация нового листа: устанавливаем количество ячеек в 0
void initialize_leaf_node(void* node) 
{
    set_node_type(node, NODE_LEAF);
    set_node_root(node, false);
    *leaf_node_num_cells(node) = 0;
    *leaf_node_next_leaf(node) = 0;
}

// Вставка нового узла в упорядоченный массив внутри листа
void leaf_node_insert(Cursor *cursor, uint32_t key, Row *value) 
{
    void* node = get_page(cursor->table->pager, cursor->page_num);

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

void print_row(Row* row) 
{
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

void print_constants() 
{
    printf("ROW_SIZE: %d\n", ROW_SIZE);
    printf("COMMON_NODE_HEADER_SIZE: %d\n", COMMON_NODE_HEADER_SIZE);
    printf("LEAF_NODE_HEADER_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
    printf("LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_CELL_SIZE);
    printf("LEAF_NODE_SPACE_FOR_CELLS: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
    printf("LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
}

void indent(uint32_t level) 
{
    for (uint32_t i = 0; i < level; i++) 
    {
        printf("  ");
    }
}

NodeType get_node_type(void* node) 
{
    uint8_t value = *((uint8_t*)(node + NODE_TYPE_OFFSET));
    return (NodeType)value;
}
    
void set_node_type(void* node, NodeType type) 
{
    uint8_t value = type;
    *((uint8_t*)(node + NODE_TYPE_OFFSET)) = value;
}

uint32_t* node_parent(void* node) 
{ 
    return node + PARENT_POINTER_OFFSET; 
}

void leaf_node_split_and_insert(Cursor* cursor, uint32_t key, Row* value) 
{
    /*
    Create a new node and move half the cells over.
    Insert the new value in one of the two nodes.
    Update parent or create a new parent.
    */
    void* old_node = get_page(cursor->table->pager, cursor->page_num);
    uint32_t old_max = get_node_max_key(cursor->table->pager, old_node);
    uint32_t new_page_num = get_unused_page_num(cursor->table->pager);
    void* new_node = get_page(cursor->table->pager, new_page_num);
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
        void* destination_node;

        if (i >= LEAF_NODE_LEFT_SPLIT_COUNT) 
        {
            destination_node = new_node;
        } else {
            destination_node = old_node;
        }
        uint32_t index_within_node = i % LEAF_NODE_LEFT_SPLIT_COUNT;
        void* destination = leaf_node_cell(destination_node, index_within_node);
        if (i == cursor->cell_num) 
        {
            serialize_row(value, destination);
            serialize_row(value, leaf_node_value(destination_node, index_within_node));
            *leaf_node_key(destination_node, index_within_node) = key;
        } 
        else if (i > cursor->cell_num) 
        {
            memcpy(destination, leaf_node_cell(old_node, i - 1), LEAF_NODE_CELL_SIZE);
        } else 
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

uint32_t* internal_node_num_keys(void* node) 
{
      return node + INTERNAL_NODE_NUM_KEYS_OFFSET;
}
    
uint32_t* internal_node_right_child(void* node) 
{
    return node + INTERNAL_NODE_RIGHT_CHILD_OFFSET;
}

uint32_t* internal_node_cell(void* node, uint32_t cell_num) 
{
    return node + INTERNAL_NODE_HEADER_SIZE + cell_num * INTERNAL_NODE_CELL_SIZE;
}

uint32_t* internal_node_child(void* node, uint32_t child_num) 
{
    uint32_t num_keys = *internal_node_num_keys(node);
    if (child_num > num_keys) 
    {
    printf("Tried to access child_num %d > num_keys %d\n", child_num, num_keys);
    exit(EXIT_FAILURE);
    } else if (child_num == num_keys) 
    {
    return internal_node_right_child(node);
    } else 
    {
    return internal_node_cell(node, child_num);
    }
}

uint32_t* internal_node_key(void* node, uint32_t key_num) {
    return internal_node_cell(node, key_num) + INTERNAL_NODE_CHILD_SIZE;
}

void update_internal_node_key(void* node, uint32_t old_key, uint32_t new_key) 
{
    uint32_t old_child_index = internal_node_find_child(node, old_key);
    *internal_node_key(node, old_child_index) = new_key;
}

bool is_node_root(void* node) 
{
    uint8_t value = *((uint8_t*)(node + IS_ROOT_OFFSET));
    return (bool)value;
}
    
void set_node_root(void* node, bool is_root) 
{
    uint8_t value = is_root;
    *((uint8_t*)(node + IS_ROOT_OFFSET)) = value;
}

void initialize_internal_node(void* node) 
{
    set_node_type(node, NODE_INTERNAL);
    set_node_root(node, false);
    *internal_node_num_keys(node) = 0;
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
    /* Left child has data copied from old root */
    memcpy(left_child, root, PAGE_SIZE); // Копируем старый корень  в левого ребенка.
    set_node_root(left_child, false); // Отмечаем, что он больше не корень.
    /* Root node is a new internal node with one key and two children */
    initialize_internal_node(root); // Инициализируем текущую страницу root как новый внутренний узел (internal node).
    set_node_root(root, true); // Обозначаем его как новый корень.
    *internal_node_num_keys(root) = 1; // У нового корня только один ключ.
    *internal_node_child(root, 0) = left_child_page_num; // Первый ребёнок нового корня — это левый узел (старый корень).
    uint32_t left_child_max_key = get_node_max_key(table->pager, left_child); // Извлекаем максимальный ключ из левого узла и записываем его в корень.
    *internal_node_key(root, 0) = left_child_max_key; // Это ключ, который помогает навигации: всё, что ≤ этому значению — идёт в левое поддерево.
    *internal_node_right_child(root) = right_child_page_num; // Устанавливаем правого ребёнка корня — это тот узел, что получился из сплита.
    *node_parent(left_child) = table->root_page_num;
    *node_parent(right_child) = table->root_page_num;
}

