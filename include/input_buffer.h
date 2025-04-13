#ifndef INPUT_BUFFER_H
#define INPUT_BUFFER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

// Структура для хранения буфера ввода
typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

// Функции
InputBuffer* new_input_buffer();                    // Create buffer
void print_prompt(bool new_command);                // Print sqlcs50> and ...>
void read_input(InputBuffer* input_buffer);        // INput
void close_input_buffer(InputBuffer* input_buffer); // Free all allocation memory
ssize_t my_getline(char **lineptr, size_t *n, FILE *stream);  // Чтение строки с буфером

#endif