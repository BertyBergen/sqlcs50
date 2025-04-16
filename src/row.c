#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/row.h"
#include "../include/table.h"

#define COLUMN_USERNAME_SIZE 32 
#define COLUMN_EMAIL_SIZE 255 
#define PAGE_SIZE 4096  // Paging: Разделяем всё хранилище на блоки фиксированного размера (страницы), например 4096 байт.
#define TABLE_MAX_PAGES 100 //pages[] — массив указателей на эти блоки. Загружаем их по мере необходимости.


#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

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
const uint32_t LEAF_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE;

//Leaf Node Body Layout
const uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_KEY_OFFSET = 0;
const uint32_t LEAF_NODE_VALUE_SIZE = ROW_SIZE;
const uint32_t LEAF_NODE_VALUE_OFFSET = LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;
const uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
const uint32_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_MAX_CELLS = LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;


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
    return (uint32_t*)(node + LEAF_NODE_NUM_CELLS_OFFSET);
}

// Возвращает указатель на начало ячейки с номером cell_num
void* leaf_node_cell(void* node, uint32_t cell_num) 
{
    return node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

// Возвращает указатель на ключ в ячейке
uint32_t* leaf_node_key(void* node, uint32_t cell_num) 
{
    return (uint32_t*)leaf_node_cell(node, cell_num);
}

// Возвращает указатель на значение в ячейке
void* leaf_node_value(void* node, uint32_t cell_num)
{
    return leaf_node_cell(node, cell_num) + LEAF_NODE_KEY_SIZE;
}

// Инициализация нового листа: устанавливаем количество ячеек в 0
void initialize_leaf_node(void* node) 
{
    *leaf_node_num_cells(node) = 0;
}


void leaf_node_insert(Cursor* cursor, uint32_t key, Row* value) {
  void* node = get_page(cursor->table->pager, cursor->page_num);

  uint32_t num_cells = *leaf_node_num_cells(node);
  if (num_cells >= LEAF_NODE_MAX_CELLS) {
    // Node full
    printf("Need to implement splitting a leaf node.\n");
    exit(EXIT_FAILURE);
  }

  if (cursor->cell_num < num_cells) {
    // Make room for new cell
    for (uint32_t i = num_cells; i > cursor->cell_num; i--) {
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

void print_leaf_node(void* node) 
{
    uint32_t num_cells = *leaf_node_num_cells(node);
    printf("leaf (size %d)\n", num_cells);
    for (uint32_t i = 0; i < num_cells; i++) {
      uint32_t key = *leaf_node_key(node, i);
      printf("  - %d : %d\n", i, key);
    }
}