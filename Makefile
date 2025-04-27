# Компилятор
CC = gcc

# Опции компилятора
CFLAGS = -Wall -Wextra -pedantic -std=c99

# Пути для поиска заголовочных файлов
INCLUDES = -I./include

# Исходники
SRC = src/main.c src/input_buffer.c src/meta_commands.c src/execute.c src/prepare.c src/table.c src/cursor.c src/pager.c src/row.c src/btree.c 

# Объектные файлы
OBJ = main.o input_buffer.o meta_commands.o execute.o prepare.o table.o cursor.o pager.o row.o btree.o

# Имя исполняемого файла
TARGET = qwe

# Правило по умолчанию для сборки
all: $(TARGET)

# Правило для компиляции и линковки
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJ) -o $(TARGET)


# Правило для компиляции исходников в объектные файлы
%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Правило для очистки
clean:
	rm -f $(TARGET) $(OBJ)

# Правило для пересборки
rebuild: clean all

.PHONY: all clean rebuild
