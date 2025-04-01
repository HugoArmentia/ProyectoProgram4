# Compilador
CC = gcc

# Flags de compilación
CFLAGS = -Wall -Wextra -Iinclude/server -Iinclude/common

# Archivos fuente
SRC = src/server/main.c \
      src/server/menu.c \
      src/server/usuarios.c \
      src/server/citas.c \
      src/server/historial.c \
      src/server/logs.c \
      src/server/database.c \
      src/server/config.c \
      src/common/utils.c

# Archivos objeto
OBJ = $(SRC:.c=.o)

# Nombre del ejecutable
EXEC = SRCM.exe

# Regla por defecto
all: $(EXEC)

# Regla para compilar el ejecutable
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

# Regla para compilar cada archivo .c a .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regla para limpiar archivos compilados
clean:
	del /Q $(OBJ) $(EXEC) 2>nul || rm -f $(OBJ) $(EXEC)

# Regla para limpiar archivos objeto
cleanobj:
	del /Q $(OBJ) 2>nul || rm -f $(OBJ)
