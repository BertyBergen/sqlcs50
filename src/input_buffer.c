#include "../include/input_buffer.h"


InputBuffer* new_input_buffer() {
    InputBuffer* input_buffer = malloc(sizeof(InputBuffer));
    if (input_buffer == NULL) {
        // Обработка ошибки, например, завершение программы
        exit(1); 
    }
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;
    return input_buffer;
}

void print_prompt(bool new_command) {

    if (new_command) {
        printf("sqlcs50 > ");
    } else {
        printf("   ...> ");
    }
}

void read_input(InputBuffer* input_buffer) {
    ssize_t bytes_read = my_getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    if (bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = '\0';
}

void close_input_buffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

ssize_t my_getline(char **lineptr, size_t *n, FILE *stream) {
    if (*lineptr == NULL) {
        *n = 128;  // Начальный размер буфера
        *lineptr = malloc(*n);
        if (*lineptr == NULL) {
            return -1;  // Ошибка выделения памяти
        }
    }

    
    char *ptr = *lineptr;
    int c = 0;
    size_t i = 0;  // Используем size_t для индекса
    ptr[i] = 0; // Используем, для правильной стилизации инпута. 

    bool new_command = true;

    print_prompt(new_command);


    while ((c = fgetc(stream)) != EOF) {
        
        if (c == ';')
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

                ptr[i++] = ' '; // Ставим пробелы, чтобы можно было потом удобнее спарсить значения. 
            }
            continue;
        }

        if (i + 1 >= *n) {  // Если буфер переполнен
            *n *= 2;  // Увеличиваем размер буфера
            *lineptr = realloc(*lineptr, *n);
            if (*lineptr == NULL) {
                return -1;  // Ошибка перераспределения памяти
            }
            ptr = *lineptr;  // Обновляем указатель на строку
        }
        ptr[i++] = c;
    }

    if (c == EOF && i == 0) {  // Если не было считано ни одного символа
        return -1;
    }

    ptr[i++] = '\0';  // Завершаем строку символом '\0'
    return i;  // Возвращаем количество считанных байт
}
