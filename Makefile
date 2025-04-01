# Makefile para compilar el proyecto SRCM

CC = gcc
CFLAGS = -Wall -Wextra -Iinclude/server -Iinclude/common
LDFLAGS = -lm
TARGET = SRCM

# Archivos fuente (src/server y src/common)
SRC_SERVER = src/server/main.c src/server/menu.c src/server/usuarios.c src/server/citas.c \
             src/server/historial.c src/server/logs.c src/server/database.c src/server/config.c

SRC_COMMON = src/common/utils.c

# Archivos objeto generados por la compilación
OBJ_SERVER = $(SRC_SERVER:.c=.o)
OBJ_COMMON = $(SRC_COMMON:.c=.o)

# Regla por defecto para compilar todo el proyecto
all: $(TARGET)

$(TARGET): $(OBJ_SERVER) $(OBJ_COMMON)
	$(CC) $(OBJ_SERVER) $(OBJ_COMMON) -o $(TARGET) $(LDFLAGS)

# Regla para compilar archivos .c a archivos .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos generados
clean:
	rm -f $(OBJ_SERVER) $(OBJ_COMMON) $(TARGET)

# Ejecutar el programa
run:
	./$(TARGET)
