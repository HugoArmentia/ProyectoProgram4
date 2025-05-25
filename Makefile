# Compiladores
CC = gcc
CXX = g++

# Flags
CFLAGS = -Wall -Wextra -Iinclude -Iinclude/server -Iinclude/common
CXXFLAGS = -Wall -Wextra -Iinclude

# Librerías
LDFLAGS = -Llib
LIBS = -lsqlite3 -lws2_32

# Rutas
SERVER_SRC = $(wildcard src/server/*.c) src/common/utils.c
SERVER_OBJ = $(SERVER_SRC:.c=.o)
SERVER_BIN = servidor.exe

CLIENT_SRC = src/client/SRCM_Client.cpp
CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)
CLIENT_BIN = SRCM_Client.exe

# Reglas principales
all: $(SERVER_BIN) $(CLIENT_BIN)

$(SERVER_BIN): $(SERVER_OBJ)
	$(CC) $(SERVER_OBJ) -o $@ $(LDFLAGS) $(LIBS)

$(CLIENT_BIN): $(CLIENT_OBJ)
	$(CXX) $(CLIENT_OBJ) -o $@ -lws2_32

# Reglas de compilación
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpiar objetos y ejecutables
clean:
	@echo "Eliminando objetos y ejecutables..."
	-del /Q $(SERVER_OBJ) $(CLIENT_OBJ) $(SERVER_BIN) $(CLIENT_BIN) 2>nul || rm -f $(SERVER_OBJ) $(CLIENT_OBJ) $(SERVER_BIN) $(CLIENT_BIN)

# Limpiar solo objetos
cleanobj:
	@echo "Eliminando archivos .o..."
	-del /Q $(SERVER_OBJ) $(CLIENT_OBJ) 2>nul || rm -f $(SERVER_OBJ) $(CLIENT_OBJ)
