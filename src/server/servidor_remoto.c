// servidor_remoto.c - mejoras finales para médico y validación de tipo
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <sqlite3.h>
#include <time.h>

// #pragma comment(lib, "ws2_32.lib")

#define PORT 5000
#define DB_PATH "citas_medicas.db"
#define LOG_PATH "log.txt"
#define BUFFER_SIZE 2048

sqlite3 *db;
char tipo_usuario[20] = "";
int id_usuario = -1;

void registrar_log_evento(const char *tipo, const char *descripcion, int usuario_id) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO logs (tipo_evento, descripcion, usuario_id) VALUES (?, ?, ?)";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, tipo, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, descripcion, -1, SQLITE_STATIC);
        if (usuario_id >= 0)
            sqlite3_bind_int(stmt, 3, usuario_id);
        else
            sqlite3_bind_null(stmt, 3);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        // Mostrar también por consola
        printf("[LOG - %s] %s (usuario_id=%d)\n", tipo, descripcion, usuario_id);
        fflush(stdout); // para asegurar que se muestre inmediatamente
    }
}

void limpiar_buffer(char *str) {
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r' || str[len - 1] == ' ')) {
        str[--len] = '\0';
    }
}

void normalizar_tipo(char *tipo) {
    if (strcmp(tipo, "medico") == 0 || strcmp(tipo, "Medico") == 0)
        strcpy(tipo, "Medico");
    else if (strcmp(tipo, "paciente") == 0 || strcmp(tipo, "Paciente") == 0)
        strcpy(tipo, "Paciente");
    else if (strcmp(tipo, "admin") == 0 || strcmp(tipo, "Admin") == 0)
    strcpy(tipo, "Admin");
    else
        tipo[0] = '\0';
}

int enviar(SOCKET sock, const char *msg) {
    return send(sock, msg, strlen(msg), 0);
}

int login_usuario(const char *usuario, const char *password) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, tipo FROM Usuario WHERE nombre = ? AND password = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return 0;
    sqlite3_bind_text(stmt, 1, usuario, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
    int ok = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        id_usuario = sqlite3_column_int(stmt, 0);
        strcpy(tipo_usuario, (const char *)sqlite3_column_text(stmt, 1));
        limpiar_buffer(tipo_usuario);
        normalizar_tipo(tipo_usuario);
        ok = strlen(tipo_usuario) > 0;
    }
    sqlite3_finalize(stmt);
    return ok;
}

int registrar_usuario(const char *nombre, const char *tipo, const char *password) {
    char tipo_normalizado[20];
    strcpy(tipo_normalizado, tipo);
    normalizar_tipo(tipo_normalizado);
    if (strlen(tipo_normalizado) == 0) return 0;
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO Usuario(nombre, tipo, password) VALUES (?, ?, ?)";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return 0;
    sqlite3_bind_text(stmt, 1, nombre, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, tipo_normalizado, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, password, -1, SQLITE_STATIC);
    int result = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return result;
}

void responder_citas(SOCKET sock, const char *sql, const char *label) {
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        enviar(sock, "Error al preparar consulta\n");
        return;
    }
    sqlite3_bind_int(stmt, 1, id_usuario);
    char buffer[BUFFER_SIZE] = "";
    int hayResultados = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        hayResultados = 1;
        char linea[256];
        snprintf(linea, sizeof(linea), "ID: %d | %s: %d | Fecha: %s | Estado: %s\n",
            sqlite3_column_int(stmt, 0),
            label,
            sqlite3_column_int(stmt, 1),
            sqlite3_column_text(stmt, 2),
            sqlite3_column_text(stmt, 3));
        strcat(buffer, linea);
    }
    if (!hayResultados) strcat(buffer, "No hay citas disponibles.\n");
    enviar(sock, buffer);
    sqlite3_finalize(stmt);
}

void manejar_comando(SOCKET sock, char *comando) {
    limpiar_buffer(comando);
    char *cmd = strtok(comando, " ");

    if (!cmd) { enviar(sock, "Comando no reconocido\n"); return; }

    if (strcmp(cmd, "LOGIN") == 0) {
        char *u = strtok(NULL, " ");
        char *p = strtok(NULL, " ");
        if (u && p && login_usuario(u, p)) {
            char r[64];
            snprintf(r, sizeof(r), "OK %s\n", tipo_usuario);
            registrar_log_evento("Login", "Inicio de sesión exitoso", id_usuario);
            enviar(sock, r);
        } else {
            registrar_log_evento("Login fallido", "Intento de login fallido", -1);
            enviar(sock, "ERROR\n");
        }
    } else if (strcmp(cmd, "REGISTER") == 0) {
        char *n = strtok(NULL, " ");
        char *t = strtok(NULL, " ");
        char *p = strtok(NULL, " ");
        if (n && t && p && registrar_usuario(n, t, p)) {
            registrar_log_evento("Registro", "Nuevo usuario registrado", -1);
            enviar(sock, "Usuario registrado\n");
        } else {
            enviar(sock, "Error al registrar usuario\n");
        }
    } else if (strcmp(cmd, "BOOK") == 0) {
        if (strcmp(tipo_usuario, "Paciente") != 0) { enviar(sock, "Solo pacientes\n"); return; }
        char *med = strtok(NULL, " ");
        char *fecha = strtok(NULL, "");
        sqlite3_stmt *stmt;
        const char *sql = "INSERT INTO citas(paciente_id, medico_id, fecha, estado) VALUES (?, ?, ?, 'Programada')";
        if (!med || !fecha || sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return;
        sqlite3_bind_int(stmt, 1, id_usuario);
        sqlite3_bind_text(stmt, 2, med, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, fecha, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            registrar_log_evento("Cita", "Paciente reservó una cita", id_usuario);
            enviar(sock, "Cita reservada\n");
        } else {
            enviar(sock, "Error al reservar\n");
        }
        sqlite3_finalize(stmt);
    } else if (strcmp(cmd, "MODIFY") == 0) {
        char *id = strtok(NULL, " ");
        char *fecha = strtok(NULL, "");
        sqlite3_stmt *stmt;
        const char *sql = "UPDATE citas SET fecha = ? WHERE id = ? AND paciente_id = ?";
        if (!id || !fecha || sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return;
        sqlite3_bind_text(stmt, 1, fecha, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, id, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, id_usuario);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            registrar_log_evento("Cita", "Paciente modificó una cita", id_usuario);
            enviar(sock, "Cita modificada\n");
        } else {
            enviar(sock, "Error al modificar\n");
        }
        sqlite3_finalize(stmt);
    } else if (strcmp(cmd, "CANCEL") == 0) {
        char *id = strtok(NULL, " ");
        sqlite3_stmt *stmt;
        const char *sql = "UPDATE citas SET estado = 'Cancelada' WHERE id = ? AND paciente_id = ?";
        if (!id || sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return;
        sqlite3_bind_text(stmt, 1, id, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, id_usuario);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            registrar_log_evento("Cita", "Paciente canceló una cita", id_usuario);
            enviar(sock, "Cita cancelada\n");
        } else {
            enviar(sock, "Error al cancelar\n");
        }
        sqlite3_finalize(stmt);
    } else if (strcmp(cmd, "LIST-CITAS") == 0) {
        const char *sql = (strcmp(tipo_usuario, "Medico") == 0) ?
            "SELECT id, paciente_id, fecha, estado FROM citas WHERE medico_id = ?" :
            "SELECT id, medico_id, fecha, estado FROM citas WHERE paciente_id = ?";
        const char *label = (strcmp(tipo_usuario, "Medico") == 0) ? "ID Paciente" : "ID Medico";
        responder_citas(sock, sql, label);
    } else if (strcmp(cmd, "HISTORY") == 0) {
        const char *sql = (strcmp(tipo_usuario, "Medico") == 0) ?
            "SELECT id, paciente_id, fecha, estado FROM citas WHERE medico_id = ?" :
            "SELECT id, medico_id, fecha, estado FROM citas WHERE paciente_id = ?";
        const char *label = (strcmp(tipo_usuario, "Medico") == 0) ? "ID Paciente" : "ID Medico";
        responder_citas(sock, sql, label);
    } else if (strcmp(cmd, "COMPLETAR") == 0) {
        char *id = strtok(NULL, " ");
        sqlite3_stmt *stmt;
        const char *sql = "UPDATE citas SET estado = 'Completada' WHERE id = ? AND medico_id = ?";
        if (!id || sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return;
        sqlite3_bind_text(stmt, 1, id, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, id_usuario);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            registrar_log_evento("Cita", "Médico completó una cita", id_usuario);
            enviar(sock, "Cita marcada como completada\n");
        } else {
            enviar(sock, "Error al completar\n");
        }
        sqlite3_finalize(stmt);
    } else if (strcmp(cmd, "LOGOUT") == 0) {
        tipo_usuario[0] = '\0';
        id_usuario = -1;
        enviar(sock, "Sesion cerrada\n");
    } else if (strcmp(cmd, "LIST-USERS") == 0) {
        if (strcmp(tipo_usuario, "Admin") != 0) {
            enviar(sock, "Acceso denegado\n");
            return;
        }

        sqlite3_stmt *stmt;
        const char *sql = "SELECT id, nombre, tipo FROM Usuario";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
            enviar(sock, "Error al listar usuarios\n");
            return;
        }

        char buffer[BUFFER_SIZE] = "";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            char linea[256];
            snprintf(linea, sizeof(linea), "ID: %d | Nombre: %s | Tipo: %s\n",
                sqlite3_column_int(stmt, 0),
                sqlite3_column_text(stmt, 1),
                sqlite3_column_text(stmt, 2));
            strcat(buffer, linea);
        }
        sqlite3_finalize(stmt);
        enviar(sock, buffer[0] ? buffer : "No hay usuarios\n");
    }
    else if (strcmp(cmd, "VIEW-LOGS") == 0) {
        if (strcmp(tipo_usuario, "Admin") != 0) {
            enviar(sock, "Acceso denegado\n");
            return;
        }

        sqlite3_stmt *stmt;
        const char *sql = "SELECT id, tipo_evento, descripcion, usuario_id, fecha_evento FROM logs ORDER BY fecha_evento DESC LIMIT 100";
        // Limitamos a los últimos 100 logs para no saturar la respuesta
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
            enviar(sock, "Error al consultar logs\n");
            return;
        }

        char buffer[BUFFER_SIZE] = "";
        int hayLogs = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            hayLogs = 1;
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char *tipo_evento = sqlite3_column_text(stmt, 1);
            const unsigned char *descripcion = sqlite3_column_text(stmt, 2);
            int usuario_id = sqlite3_column_int(stmt, 3);
            const unsigned char *fecha_evento = sqlite3_column_text(stmt, 4);

            char linea[512];
            snprintf(linea, sizeof(linea),
                    "ID: %d | Tipo: %s | Descripción: %s | Usuario ID: %d | Fecha: %s\n",
                    id, tipo_evento ? (const char*)tipo_evento : "NULL",
                    descripcion ? (const char*)descripcion : "NULL",
                    usuario_id,
                    fecha_evento ? (const char*)fecha_evento : "NULL");

            if (strlen(buffer) + strlen(linea) < sizeof(buffer) - 1) {
                strcat(buffer, linea);
            } else {
                // Buffer lleno, para no cortar logs a la mitad
                break;
            }
        }
        sqlite3_finalize(stmt);

        if (!hayLogs) {
            enviar(sock, "No hay logs disponibles\n");
        } else {
            enviar(sock, buffer);
        }
    }
    else if (strcmp(cmd, "MODIFY-USER") == 0) {
        if (strcmp(tipo_usuario, "Admin") != 0) {
            enviar(sock, "Acceso denegado\n");
            return;
        }

        char *id = strtok(NULL, " ");
        char *nuevo_nombre = strtok(NULL, "");

        if (!id || !nuevo_nombre) {
            enviar(sock, "Uso: MODIFY-USER <id> <nuevo_nombre>\n");
            return;
        }

        sqlite3_stmt *stmt;
        const char *sql = "UPDATE Usuario SET nombre = ? WHERE id = ?";

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
            enviar(sock, "Error preparando la modificación\n");
            return;
        }

        sqlite3_bind_text(stmt, 1, nuevo_nombre, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, id, -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            registrar_log_evento("Admin", "Modificó un usuario", id_usuario);
            enviar(sock, "Usuario modificado correctamente\n");
        } else {
            enviar(sock, "Error al modificar usuario\n");
        }


        sqlite3_finalize(stmt);
    }
    else if (strcmp(cmd, "DELETE-USER") == 0) {
        if (strcmp(tipo_usuario, "Admin") != 0) {
            enviar(sock, "Acceso denegado\n");
            return;
        }

        char *id = strtok(NULL, " ");
        if (!id) {
            enviar(sock, "Uso: DELETE-USER <id>\n");
            return;
        }

        sqlite3_stmt *stmt;
        const char *sql = "DELETE FROM Usuario WHERE id = ?";

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
            enviar(sock, "Error preparando la eliminación\n");
            return;
        }

        sqlite3_bind_text(stmt, 1, id, -1, SQLITE_STATIC);

        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_DONE) {
            int cambios = sqlite3_changes(db);
            if (cambios > 0) {
                enviar(sock, "Usuario eliminado correctamente\n");
            } else {
                enviar(sock, "No existe usuario con ese ID\n");
            }
        } else {
            enviar(sock, "Error al eliminar usuario\n");
        }

        sqlite3_finalize(stmt);
    }
    else if (strcmp(cmd, "SHUTDOWN") == 0) {
        if (strcmp(tipo_usuario, "Admin") != 0) {
            enviar(sock, "Acceso denegado\n");
            return;
        }

        enviar(sock, "Servidor apagándose...\n");

        closesocket(sock);

        registrar_log_evento("Sistema", "Servidor apagado por admin", id_usuario);

        sqlite3_close(db);
        WSACleanup();

        exit(0);
    }

}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    SOCKET server, client;
    struct sockaddr_in saddr, caddr;
    int clen = sizeof(caddr);

    server = socket(AF_INET, SOCK_STREAM, 0);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(PORT);

    bind(server, (struct sockaddr *)&saddr, sizeof(saddr));
    listen(server, 3);

    printf("[SERVIDOR] Escuchando en puerto %d...\n", PORT);

    if (sqlite3_open(DB_PATH, &db)) {
        fprintf(stderr, "Error al abrir la BD: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    while ((client = accept(server, (struct sockaddr *)&caddr, &clen)) != INVALID_SOCKET) {
        printf("[SERVIDOR] Cliente conectado.\n");
        tipo_usuario[0] = '\0';
        id_usuario = -1;
        char buffer[BUFFER_SIZE];
        int len;
        while ((len = recv(client, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[len] = '\0';
            manejar_comando(client, buffer);
        }
        closesocket(client);
    }

    sqlite3_close(db);
    WSACleanup();
    return 0;
}

