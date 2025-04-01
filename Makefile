CC = gcc

CFLAGS = -Wall -Wextra -Iinclude -Iinclude/server -Iinclude/common

SRC = src/server/main.c \
      src/server/menu.c \
      src/server/usuarios.c \
      src/server/citas.c \
      src/server/historial.c \
      src/server/logs.c \
      src/server/database.c \
      src/server/config.c \
      src/common/utils.c \
      src/server/calendario.c

OBJ = $(SRC:.c=.o)

EXEC = SRCM.exe

LDFLAGS = -Llib

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS) -lsqlite3

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q $(OBJ) $(EXEC) 2>nul || rm -f $(OBJ) $(EXEC)

cleanobj:
	del /Q $(OBJ) 2>nul || rm -f $(OBJ)
