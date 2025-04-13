# Компилятор
CC = gcc

# Опции компилятора
CFLAGS = -Wall -Wextra -pedantic -std=c99

# Пути для поиска заголовочных файлов
INCLUDES = -I./include

# Исходники
SRC = src/main.c src/input_buffer.c src/meta_commands.c src/execute.c src/prepare.c src/table.c src/cursor.c

# Объектные файлы
OBJ = main.o src/input_buffer.o src/meta_commands.o src/execute.o src/prepare.o src/table.o src/cursor.o

# Имя исполняемого файла
TARGET = qwe

# Правило по умолчанию для сборки
all: $(TARGET)

# Правило для компиляции и линковки
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(INCLUDES) -o $(TARGET)

# Правило для компиляции исходников в объектные файлы
%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Правило для очистки
clean:
	rm -f $(TARGET) $(OBJ)

# Правило для пересборки
rebuild: clean all

.PHONY: all clean rebuild
