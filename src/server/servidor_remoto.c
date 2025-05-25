#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <sqlite3.h>
#include <time.h>
#include "server/horas.h"
#include "database.h"

// #pragma comment(lib, "ws2_32.lib")

#define PORT 5000
#define DB_PATH "citas_medicas.db"
#define LOG_PATH "log.txt"
#define BUFFER_SIZE 2048
const char *horas[] = {"08:00", "08:30", "09:00", "09:30", "10:00", "10:30", "11:00", "11:30",
                      "12:00", "12:30", "13:00", "13:30", "14:00", "14:30", "15:00", "15:30",
                      "16:00", "16:30", "17:00", "17:30", "18:00", "18:30", "19:00", "19:30"};

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

        printf("[LOG - %s] %s (usuario_id=%d)\n", tipo, descripcion, usuario_id);
        fflush(stdout);
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

void enviarHorasDisponibles(SOCKET sock, int dia, int mes, int anio);
void enviarCalendarioMensual(SOCKET sock, int mes, int anio);

void manejar_comando(SOCKET sock, char *comando) {
    limpiar_buffer(comando);
    char *cmd = strtok(comando, " ");

    if (!cmd) { enviar(sock, "Comando no reconocido\n"); return; }

    if (strcmp(cmd, "LOGIN") == 0) {
        char *u = strtok(NULL, " ");
        char *p = strtok(NULL, " ");
        if (u && p && login_usuario(u, p)) {
            char r[128];
            snprintf(r, sizeof(r), "Inicio de sesión correcto. Rol: %s\n", tipo_usuario);
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
    } else if (strcmp(cmd, "LIST-MEDICOS") == 0) {
        sqlite3_stmt *stmt;
        const char *sql = "SELECT id, nombre FROM Usuario WHERE tipo = 'Medico'";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
            enviar(sock, "Error al listar médicos\n");
            return;
        }

        char buffer[BUFFER_SIZE] = "Médicos disponibles:\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            char linea[256];
            snprintf(linea, sizeof(linea), "ID: %d | Nombre: %s\n",
                sqlite3_column_int(stmt, 0),
                sqlite3_column_text(stmt, 1));
            strncat(buffer, linea, sizeof(buffer) - strlen(buffer) - 1);
        }
        sqlite3_finalize(stmt);
        enviar(sock, buffer);
    }
    else if (strcmp(cmd, "RESERVAR") == 0) {
        if (strcmp(tipo_usuario, "Paciente") != 0) {
            enviar(sock, "Solo pacientes pueden reservar citas\n");
            return;
        }

        char *mes_str = strtok(NULL, " ");
        char *dia_str = strtok(NULL, " ");
        char *anio_str = strtok(NULL, " ");
        char *hora_str = strtok(NULL, " ");
        char *medico_str = strtok(NULL, " ");
        char *motivo = strtok(NULL, "");

        if (!mes_str || !dia_str || !anio_str || !hora_str || !medico_str || !motivo) {
            enviar(sock, "Uso: RESERVAR <mes> <dia> <anio> <indice_hora> <id_medico> <motivo>\n");
            return;
        }

        int mes = atoi(mes_str);
        int dia = atoi(dia_str);
        int anio = atoi(anio_str);
        int indiceHora = atoi(hora_str);
        int medico_id = atoi(medico_str);

        const char *medicoSql = "SELECT COUNT(*) FROM Usuario WHERE id = ? AND tipo = 'Medico'";
        sqlite3_stmt *medicoStmt;
        if (sqlite3_prepare_v2(db, medicoSql, -1, &medicoStmt, NULL) != SQLITE_OK) {
            enviar(sock, "Error al verificar el médico.\n");
            return;
        }
        sqlite3_bind_int(medicoStmt, 1, medico_id);
        int existeMedico = 0;
        if (sqlite3_step(medicoStmt) == SQLITE_ROW) {
            existeMedico = sqlite3_column_int(medicoStmt, 0);
        }
        sqlite3_finalize(medicoStmt);

        if (existeMedico == 0) {
            enviar(sock, "El ID de médico proporcionado no existe.\n");
            return;
        }

        if (mes < 1 || mes > 6) {
            enviar(sock, "Mes inválido. Debe ser entre 1 y 6.\n");
            return;
        }
        if (indiceHora < 0 || indiceHora >= HORAS_DISPONIBLES) {
            enviar(sock, "Hora inválida.\n");
            return;
        }

        if (dia < 1 || dia > 31) {
            enviar(sock, "Día inválido.\n");
            return;
        }

        time_t t = time(NULL);
        struct tm *actual = localtime(&t);
        int hoy_dia = actual->tm_mday;
        int hoy_mes = actual->tm_mon + 1;
        int hoy_anio = actual->tm_year + 1900;

        if ((anio < hoy_anio) ||
            (anio == hoy_anio && mes < hoy_mes) ||
            (anio == hoy_anio && mes == hoy_mes && dia < hoy_dia)) {
            enviar(sock, "No puedes reservar una cita en una fecha pasada.\n");
            return;
        }
        
        const char *checkSql = "SELECT COUNT(*) FROM citas "
                            "WHERE dia = ? AND mes = ? AND anio = ? AND fecha = ? AND medico_id = ? AND estado = 'Programada';";
        sqlite3_stmt *checkStmt;
        if (sqlite3_prepare_v2(db, checkSql, -1, &checkStmt, NULL) != SQLITE_OK) {
            enviar(sock, "Error al preparar la consulta de disponibilidad.\n");
            return;
        }
        sqlite3_bind_int(checkStmt, 1, dia);
        sqlite3_bind_int(checkStmt, 2, mes);
        sqlite3_bind_int(checkStmt, 3, anio);
        sqlite3_bind_text(checkStmt, 4, horas[indiceHora], -1, SQLITE_STATIC);
        sqlite3_bind_int(checkStmt, 5, medico_id);

        int ocupado = 0;
        if (sqlite3_step(checkStmt) == SQLITE_ROW) {
            ocupado = sqlite3_column_int(checkStmt, 0);
        }
        sqlite3_finalize(checkStmt);

        if (ocupado > 0) {
            enviar(sock, "El médico ya tiene una cita en esa hora. Elija otra hora.\n");
            return;
        }

        const char *sql = "INSERT INTO citas (paciente_id, medico_id, fecha, estado, motivo, dia, mes, anio, fecha_modificacion) "
                        "VALUES (?, ?, ?, 'Programada', ?, ?, ?, ?, datetime('now'));";
        sqlite3_stmt *stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
            enviar(sock, "Error al preparar la inserción de cita.\n");
            return;
        }

        sqlite3_bind_int(stmt, 1, id_usuario);
        sqlite3_bind_int(stmt, 2, medico_id);
        sqlite3_bind_text(stmt, 3, horas[indiceHora], -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, motivo, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 5, dia);
        sqlite3_bind_int(stmt, 6, mes);
        sqlite3_bind_int(stmt, 7, anio);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            registrar_log_evento("Cita", "Paciente reservó una cita con RESERVAR", id_usuario);
            enviar(sock, "Cita reservada con éxito\n");
        } else {
            enviar(sock, "Error al reservar cita\n");
        }

        sqlite3_finalize(stmt);
    }
    else if (strcmp(cmd, "CALENDARIO") == 0) {
        char *mes_str = strtok(NULL, " ");
        char *anio_str = strtok(NULL, " ");
        if (!mes_str || !anio_str) {
            enviar(sock, "Uso: CALENDARIO <mes> <anio>\n");
            return;
        }
        int mes = atoi(mes_str);
        int anio = atoi(anio_str);
        enviarCalendarioMensual(sock, mes, anio);
    }
    else if (strcmp(cmd, "HORAS") == 0) {
        char *dia_str = strtok(NULL, " ");
        char *mes_str = strtok(NULL, " ");
        char *anio_str = strtok(NULL, " ");
        if (!dia_str || !mes_str || !anio_str) {
            enviar(sock, "Uso: HORAS <dia> <mes> <anio>\n");
            return;
        }
        int dia = atoi(dia_str);
        int mes = atoi(mes_str);
        int anio = atoi(anio_str);
        enviarHorasDisponibles(sock, dia, mes, anio);
    }

    else if (strcmp(cmd, "MODIFY") == 0) {
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
        const char *sql;
        const char *label;

        if (strcmp(tipo_usuario, "Medico") == 0) {
            sql = "SELECT c.id, p.nombre, c.fecha, c.estado "
                "FROM citas c JOIN Usuario p ON c.paciente_id = p.id "
                "WHERE c.medico_id = ? AND c.estado = 'Programada'";
            label = "Paciente";
        } else if (strcmp(tipo_usuario, "Paciente") == 0) {
            sql = "SELECT c.id, m.nombre, c.fecha, c.estado "
                "FROM citas c JOIN Usuario m ON c.medico_id = m.id "
                "WHERE c.paciente_id = ? AND c.estado = 'Programada'";
            label = "Médico";
        } else {
            enviar(sock, "Solo médicos o pacientes pueden consultar sus citas.\n");
            return;
        }

        sqlite3_stmt *stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
            enviar(sock, "Error al preparar consulta de citas\n");
            return;
        }

        sqlite3_bind_int(stmt, 1, id_usuario);
        char buffer[BUFFER_SIZE] = "";
        int hayResultados = 0;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            hayResultados = 1;
            char linea[256];
            snprintf(linea, sizeof(linea), "ID: %d | %s: %s | Fecha: %s | Estado: %s\n",
                sqlite3_column_int(stmt, 0),
                label,
                sqlite3_column_text(stmt, 1),
                sqlite3_column_text(stmt, 2),
                sqlite3_column_text(stmt, 3));
            strcat(buffer, linea);
        }

        sqlite3_finalize(stmt);
        if (!hayResultados) strcat(buffer, "No hay citas programadas.\n");
        enviar(sock, buffer);

    } else if (strcmp(cmd, "HISTORY") == 0) {
        const char *sql;
        const char *label;

        if (strcmp(tipo_usuario, "Medico") == 0) {
            sql = "SELECT c.id, p.nombre, c.fecha, c.estado "
                "FROM citas c JOIN Usuario p ON c.paciente_id = p.id "
                "WHERE c.medico_id = ? AND c.estado != 'Programada'";
            label = "Paciente";
        } else if (strcmp(tipo_usuario, "Paciente") == 0) {
            sql = "SELECT c.id, m.nombre, c.fecha, c.estado "
                "FROM citas c JOIN Usuario m ON c.medico_id = m.id "
                "WHERE c.paciente_id = ? AND c.estado != 'Programada'";
            label = "Médico";
        } else {
            enviar(sock, "Solo médicos o pacientes pueden consultar su historial.\n");
            return;
        }

        sqlite3_stmt *stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
            enviar(sock, "Error al preparar consulta del historial\n");
            return;
        }

        sqlite3_bind_int(stmt, 1, id_usuario);
        char buffer[BUFFER_SIZE] = "";
        int hayResultados = 0;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            hayResultados = 1;
            char linea[256];
            snprintf(linea, sizeof(linea), "ID: %d | %s: %s | Fecha: %s | Estado: %s\n",
                sqlite3_column_int(stmt, 0),
                label,
                sqlite3_column_text(stmt, 1),
                sqlite3_column_text(stmt, 2),
                sqlite3_column_text(stmt, 3));
            strcat(buffer, linea);
        }

        sqlite3_finalize(stmt);
        if (!hayResultados) strcat(buffer, "No hay historial de citas.\n");
        enviar(sock, buffer);

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

    crearTablas();

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

void enviarHorasDisponibles(SOCKET sock, int dia, int mes, int anio) {
    char buffer[BUFFER_SIZE];
    buffer[0] = '\0';

    const char *sql = "SELECT fecha FROM citas WHERE dia = ? AND mes = ? AND anio = ? AND estado = 'Programada';";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        snprintf(buffer, sizeof(buffer), "Error al preparar consulta de horas: %s\n", sqlite3_errmsg(db));
        enviar(sock, buffer);
        return;
    }

    sqlite3_bind_int(stmt, 1, dia);
    sqlite3_bind_int(stmt, 2, mes);
    sqlite3_bind_int(stmt, 3, anio);

    int usadas[HORAS_DISPONIBLES] = {0};

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *horaReservada = (const char *)sqlite3_column_text(stmt, 0);
        for (int i = 0; i < HORAS_DISPONIBLES; i++) {
            if (strcmp(horas[i], horaReservada) == 0) {
                usadas[i] = 1;
                break;
            }
        }
    }

    sqlite3_finalize(stmt);

    for (int i = 0; i < HORAS_DISPONIBLES; i++) {
        char linea[64];
        if (usadas[i])
            snprintf(linea, sizeof(linea), "%d: %s (Reservado)\n", i, horas[i]);
        else
            snprintf(linea, sizeof(linea), "%d: %s (Disponible)\n", i, horas[i]);
        strncat(buffer, linea, sizeof(buffer) - strlen(buffer) - 1);
    }

    enviar(sock, buffer);
}


void enviarCalendarioMensual(SOCKET sock, int mes, int anio) {
    char buffer[2048];
    snprintf(buffer, sizeof(buffer), "\nCalendario para %d-%d\n  Lu Ma Mi Ju Vi Sa Do\n", mes, anio);

    time_t t = time(NULL);
    struct tm *fechaActual = localtime(&t);
    int diaHoy = fechaActual->tm_mday;
    int mesActual = fechaActual->tm_mon + 1;
    int anioActual = fechaActual->tm_year + 1900;

    struct tm t_ini = {0};
    t_ini.tm_mday = 1;
    t_ini.tm_mon = mes - 1;
    t_ini.tm_year = anio - 1900;
    mktime(&t_ini);

    int diaInicio = t_ini.tm_wday == 0 ? 7 : t_ini.tm_wday;
    int diasEnMes = (mes == 2) ? (anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0) ? 29 : 28) :
                     (mes == 4 || mes == 6 || mes == 9 || mes == 11 ? 30 : 31);

    for (int i = 1; i < diaInicio; i++) strcat(buffer, "   ");

    for (int dia = 1; dia <= diasEnMes; dia++) {
        int reservado = 0;
        const char *sql = "SELECT COUNT(*) FROM citas WHERE dia = ? AND mes = ? AND anio = ? AND estado = 'Programada';";
        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, dia);
            sqlite3_bind_int(stmt, 2, mes);
            sqlite3_bind_int(stmt, 3, anio);

            if (sqlite3_step(stmt) == SQLITE_ROW) {
                if (sqlite3_column_int(stmt, 0) > 0) reservado = 1;
            }
            sqlite3_finalize(stmt);
        }

        char diaStr[8];
        if (anio == anioActual && mes == mesActual && dia < diaHoy) {
            snprintf(diaStr, sizeof(diaStr), " -- ");
        } else if (reservado) {
            snprintf(diaStr, sizeof(diaStr), "[%2d]", dia);
        } else {
            snprintf(diaStr, sizeof(diaStr), " %2d ", dia);
        }
        strcat(buffer, diaStr);

        if ((dia + diaInicio - 1) % 7 == 0 || dia == diasEnMes) strcat(buffer, "\n");
    }

    enviar(sock, buffer);
}