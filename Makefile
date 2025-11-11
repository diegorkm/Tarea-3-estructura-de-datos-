CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
TARGET = bio

SOURCES = main.c trie.c
HEADERS = trie.h
OBJS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)
	@echo "Compilacion exitosa: ./$(TARGET)"

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

run: $(TARGET)
	./$(TARGET)