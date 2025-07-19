#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/input_buffer.h"

InputBuffer* new_input_buffer() {
    InputBuffer* input_buffer = malloc(sizeof(InputBuffer));
    if (input_buffer == NULL) {
        
        exit(1); 
    }
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;
    return input_buffer;
}

void print_prompt(bool new_command) {

    if (new_command) {
        printf("sqlcs50> ");
    } else {
        printf("   ...> ");
    }
}

void read_input(InputBuffer *input_buffer) {
    ssize_t bytes_read = my_getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    if (bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = '\0';
}

void close_input_buffer(InputBuffer *input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

ssize_t my_getline(char **lineptr, size_t *n, FILE *stream) {
    if (*lineptr == NULL) 
    {
        *n = 128;  // Начальный размер буфера
        *lineptr = malloc(*n);
        if (*lineptr == NULL) 
        {
            return -1;  
        }
    }

    
    char *ptr = *lineptr;
    int c = 0;
    size_t i = 0;  
    ptr[i] = 0; // Используем, для правильной стилизации input. 

    bool new_command = true;

    print_prompt(new_command);

    bool is_meta_command =  false;

    while ((c = fgetc(stream)) != EOF) {
        
        if (i == 0 && c == '.')
        {
            is_meta_command = true;
        }

        if (is_meta_command && c == '\n')
        {
            break;
        }
        
        if (!is_meta_command &&  c == ';')
        {
            break;
        }
        
        
        if (c == '\n') 
        {   
            if(ptr[i])
            {
                new_command = false;
                print_prompt(new_command);
            }
            if (i > 1)
            {

                ptr[i++] = ' ';  
            }
            continue;
        }

        if (i + 1 >= *n) {  
            *n *= 2;  
            *lineptr = realloc(*lineptr, *n);
            if (*lineptr == NULL) {
                return -1;  
            }
            ptr = *lineptr;  
        }
        ptr[i++] = c;
    }

    if (c == EOF && i == 0) {  
        return -1;
    }

    ptr[i++] = '\0';  
    return i;  
}
