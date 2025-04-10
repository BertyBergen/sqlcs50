# Компилятор
CC = gcc

# Опции компилятора
CFLAGS = -Wall -Wextra -pedantic -std=c99

# Пути для поиска заголовочных файлов
INCLUDES = -I./include

# Источник исходных файлов
SRC = main.c src/input_buffer.c

# Имя исполняемого файла
TARGET = sqlcs50

# Правило по умолчанию для сборки
all: $(TARGET)

# Правило для компиляции и линковки
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(INCLUDES) -o $(TARGET)

# Правило для очистки
clean:
	rm -f $(TARGET)

# Правило для пересборки
rebuild: clean all

.PHONY: all clean rebuild
