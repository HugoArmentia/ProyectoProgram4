#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "logs.h"
#include "utils.h"
#include "database.h"

void registrarLog(const char *tipo_evento, const char *descripcion, int usuario_id) {
    char fecha_evento[20];
    obtenerFechaHoraActual(fecha_evento);

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO logs (tipo_evento, descripcion, usuario_id, fecha_evento) VALUES (?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, tipo_evento, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, descripcion, -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, usuario_id);
        sqlite3_bind_text(stmt, 4, fecha_evento, -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            printf("Error al registrar log: %s\n", sqlite3_errmsg(db));
        }

        sqlite3_finalize(stmt);
    } else {
        printf("Error al preparar registro de log: %s\n", sqlite3_errmsg(db));
    }
}

void listarLogs() {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, tipo_evento, descripcion, usuario_id, fecha_evento FROM logs ORDER BY id DESC";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error al preparar SELECT de logs: %s\n", sqlite3_errmsg(db));
        return;
    }

    printf("======= LISTADO DE LOGS =======\n");

    int hayLogs = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        hayLogs = 1;
        printf("ID: %d\n", sqlite3_column_int(stmt, 0));
        printf("Tipo de Evento: %s\n", sqlite3_column_text(stmt, 1));
        printf("Descripción: %s\n", sqlite3_column_text(stmt, 2));
        printf("ID de Usuario: %d\n", sqlite3_column_int(stmt, 3));
        printf("Fecha del Evento: %s\n", sqlite3_column_text(stmt, 4));
        printf("------------------------------\n");
    }

    if (!hayLogs) {
        printf("No hay logs registrados.\n");
    }

    sqlite3_finalize(stmt);
}
