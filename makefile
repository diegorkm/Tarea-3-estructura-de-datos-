# Compilador y flags
CC      := gcc
CFLAGS  := -Wall -Wextra -Werror -g -O2 -std=c99
# -Werror: trata warnings como errores (muy recomendado en proyectos finales)
# -O2: optimización sin sacrificar depuración
# -g: incluye info de debug

# Nombre del ejecutable
TARGET  := bio
# Archivos fuente y objetos
SOURCES := main.c trie.c
OBJECTS := $(SOURCES:.c=.o)

# Regla principal
all: $(TARGET)
	@echo "Compilación exitosa → ./$(TARGET)"

# Linkeo final
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

# Compilación de cada .c → .o (con dependencia automática de headers)
%.o: %.c trie.h
	$(CC) $(CFLAGS) -c $< -o $@

# Limpieza
clean:
	rm -f $(TARGET) $(OBJECTS) .log vgcore. 
	@echo "Limpieza completada"

# Ejecutar el programa
run: $(TARGET)
	@echo "Ejecutando $(TARGET)..."
	@./$(TARGET)

# Phony targets 
.PHONY: all clean run valgrind lines
