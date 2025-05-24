#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <sqlite3.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 5000
#define DB_PATH "citas_medicas.db"
#define BUFFER_SIZE 2048

sqlite3 *db;
char tipo_usuario[20] = "";

void escribir_log(const char *mensaje) {
    FILE *log = fopen("log.txt", "a");
    if (log) {
        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);
        char time_buf[26];
        strftime(time_buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
        fprintf(log, "[%s] %s\n", time_buf, mensaje);
        fclose(log);
    }
}

int enviar(SOCKET clientSocket, const char *msg) {
    return send(clientSocket, msg, strlen(msg), 0);
}

void limpiar_buffer(char *str) {
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r' || str[len - 1] == ' ')) {
        str[len - 1] = '\0';
        len--;
    }
}

int login_usuario(const char *usuario, const char *pass) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT tipo FROM Usuario WHERE nombre = ? AND password = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return 0;
    sqlite3_bind_text(stmt, 1, usuario, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, pass, -1, SQLITE_STATIC);
    int result = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *tipo = sqlite3_column_text(stmt, 0);
        strcpy(tipo_usuario, (const char *)tipo);
        result = 1;
    }
    sqlite3_finalize(stmt);
    return result;
}

int registrar_usuario(const char *nombre, const char *tipo, const char *pass) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO Usuario(nombre, tipo, password) VALUES (?, ?, ?)";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return 0;
    sqlite3_bind_text(stmt, 1, nombre, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, tipo, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, pass, -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

void manejar_comando(SOCKET clientSocket, char *comando) {
    limpiar_buffer(comando);
    printf("[DEBUG] Comando recibido: '%s'\n", comando);
    char *cmd = strtok(comando, " ");
    if (!cmd) { enviar(clientSocket, "Comando no reconocido\n"); return; }

    if (strcmp(cmd, "LOGIN") == 0) {
        char *usuario = strtok(NULL, " ");
        char *pass = strtok(NULL, " ");
        if (usuario && pass && login_usuario(usuario, pass)) {
            escribir_log("LOGIN correcto");
            enviar(clientSocket, "OK\n");
        } else {
            escribir_log("LOGIN fallido");
            enviar(clientSocket, "ERROR\n");
        }
    } else if (strcmp(cmd, "REGISTER") == 0) {
        char *nombre = strtok(NULL, " ");
        char *tipo = strtok(NULL, " ");
        char *pass = strtok(NULL, " ");
        if (nombre && tipo && pass && registrar_usuario(nombre, tipo, pass)) {
            escribir_log("Registro de usuario correcto");
            enviar(clientSocket, "Usuario registrado\n");
        } else {
            escribir_log("Registro fallido");
            enviar(clientSocket, "Error al registrar usuario\n");
        }
    } else if (strcmp(cmd, "BOOK") == 0) {
        if (strcmp(tipo_usuario, "Paciente") != 0) {
            enviar(clientSocket, "ERROR: Solo los pacientes pueden reservar citas\n"); return;
        }
        char *medico_id = strtok(NULL, " ");
        char *fecha = strtok(NULL, "");
        if (!medico_id || !fecha) {
            enviar(clientSocket, "ERROR: argumentos insuficientes\n"); return;
        }
        sqlite3_stmt *stmt;
        const char *sql = "INSERT INTO citas(paciente_id, medico_id, fecha, estado) VALUES (1, ?, ?, 'Programada')";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, medico_id, -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, fecha, -1, SQLITE_STATIC);
            if (sqlite3_step(stmt) == SQLITE_DONE) {
                enviar(clientSocket, "Cita reservada correctamente\n");
            } else {
                enviar(clientSocket, "Error al reservar cita\n");
            }
            sqlite3_finalize(stmt);
        } else {
            enviar(clientSocket, "Error al preparar sentencia BOOK\n");
        }
    } else if (strcmp(cmd, "LIST-CITAS") == 0) {
        sqlite3_stmt *stmt;
        const char *sql;
        if (strcmp(tipo_usuario, "Medico") == 0) {
            sql = "SELECT id, paciente_id, fecha, estado FROM citas WHERE medico_id = 2";
        } else {
            sql = "SELECT id, medico_id, fecha, estado FROM citas WHERE paciente_id = 1";
        }
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            char buffer[BUFFER_SIZE];
            buffer[0] = '\0';
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                char linea[256];
                snprintf(linea, sizeof(linea), "ID: %d | Otro ID: %d | Fecha: %s | Estado: %s\n",
                         sqlite3_column_int(stmt, 0),
                         sqlite3_column_int(stmt, 1),
                         sqlite3_column_text(stmt, 2),
                         sqlite3_column_text(stmt, 3));
                strcat(buffer, linea);
            }
            enviar(clientSocket, buffer);
            sqlite3_finalize(stmt);
        } else {
            enviar(clientSocket, "Error al listar citas\n");
        }
    } else if (strcmp(cmd, "MODIFY") == 0) {
        char *cita_id = strtok(NULL, " ");
        char *nueva_fecha = strtok(NULL, "");
        if (!cita_id || !nueva_fecha) {
            enviar(clientSocket, "ERROR: argumentos insuficientes\n"); return;
        }
        sqlite3_stmt *stmt;
        const char *sql = "UPDATE citas SET fecha = ? WHERE id = ?";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, nueva_fecha, -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, cita_id, -1, SQLITE_STATIC);
            if (sqlite3_step(stmt) == SQLITE_DONE) {
                enviar(clientSocket, "Cita modificada\n");
            } else {
                enviar(clientSocket, "Error al modificar\n");
            }
            sqlite3_finalize(stmt);
        }
    } else if (strcmp(cmd, "CANCEL") == 0) {
        char *cita_id = strtok(NULL, " ");
        if (!cita_id) {
            enviar(clientSocket, "ERROR: ID de cita faltante\n"); return;
        }
        sqlite3_stmt *stmt;
        const char *sql = "UPDATE citas SET estado = 'Cancelada' WHERE id = ?";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, cita_id, -1, SQLITE_STATIC);
            if (sqlite3_step(stmt) == SQLITE_DONE) {
                enviar(clientSocket, "Cita cancelada\n");
            } else {
                enviar(clientSocket, "Error al cancelar\n");
            }
            sqlite3_finalize(stmt);
        }
    } else if (strcmp(cmd, "HISTORY") == 0) {
        sqlite3_stmt *stmt;
        const char *sql = "SELECT id, medico_id, fecha, estado FROM citas WHERE paciente_id = 1";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            char buffer[BUFFER_SIZE];
            buffer[0] = '\0';
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                char linea[256];
                snprintf(linea, sizeof(linea), "ID: %d | Medico: %d | Fecha: %s | Estado: %s\n",
                         sqlite3_column_int(stmt, 0),
                         sqlite3_column_int(stmt, 1),
                         sqlite3_column_text(stmt, 2),
                         sqlite3_column_text(stmt, 3));
                strcat(buffer, linea);
            }
            enviar(clientSocket, buffer);
            sqlite3_finalize(stmt);
        } else {
            enviar(clientSocket, "Error al consultar historial\n");
        }
    } else if (strcmp(cmd, "LOGOUT") == 0) {
        tipo_usuario[0] = '\0';
        enviar(clientSocket, "Sesion cerrada\n");
    } else {
        enviar(clientSocket, "Comando no reconocido\n");
    }
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    struct sockaddr_in server, client;
    SOCKET s, new_socket;
    int c;

    s = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    bind(s, (struct sockaddr *)&server, sizeof(server));
    listen(s, 3);

    printf("[Servidor] Esperando conexiones en el puerto %d...\n", PORT);

    if (sqlite3_open(DB_PATH, &db)) {
        fprintf(stderr, "No se pudo abrir la base de datos: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    c = sizeof(struct sockaddr_in);
    while ((new_socket = accept(s, (struct sockaddr *)&client, &c)) != INVALID_SOCKET) {
        printf("Cliente conectado\n");
        char buffer[BUFFER_SIZE];
        int len;
        while ((len = recv(new_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
            buffer[len] = '\0';
            manejar_comando(new_socket, buffer);
        }
        closesocket(new_socket);
    }

    sqlite3_close(db);
    WSACleanup();
    return 0;
}
