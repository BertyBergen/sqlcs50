# Компилятор
CC = gcc

# Опции компилятора
CFLAGS = -Wall -Wextra -pedantic -std=c99 -g

# Пути для поиска заголовочных файлов
INCLUDES = -I./include

# Исходники
SRC = src/main.c src/input_buffer.c src/meta_commands.c src/execute.c src/prepare.c src/table.c src/cursor.c src/pager.c src/row.c src/btree.c 

# Объектные файлы
OBJ = main.o input_buffer.o meta_commands.o execute.o prepare.o table.o cursor.o pager.o row.o btree.o

# Имя исполняемого файла
TARGET = sqlcs50

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
	rm -f $(TARGET) $(OBJ) report.html

# Правило для пересборки
rebuild: clean all

# === Новые правила для тестов ===

# Запустить все тесты
test:
	pytest

# Запустить тесты и сгенерировать HTML-отчёт
html:
	pytest --html=report.html

# Автоматический перезапуск тестов при изменениях
watch:
	ptw

.PHONY: all clean rebuild test html watch
