#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "historial.h"
#include "utils.h"
#include <sqlite3.h>
#include "database.h"

void listarHistorialMedico(int medicoId) {
    const char *sql =
        "SELECT hc.id, hc.paciente_id, u.nombre, hc.fecha, hc.estado, hc.motivo "
        "FROM historial_citas hc "
        "JOIN Usuario u ON hc.paciente_id = u.id "
        "WHERE hc.medico_id = ? "
        "ORDER BY hc.anio DESC, hc.mes DESC, hc.dia DESC, hc.fecha DESC;";

    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error al preparar la consulta del historial del medico: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, medicoId);

    printf("\n======= HISTORIAL DE CITAS DEL MEDICO ID %d =======\n", medicoId);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int paciente_id = sqlite3_column_int(stmt, 1);
        const unsigned char *nombre = sqlite3_column_text(stmt, 2);
        const unsigned char *fecha = sqlite3_column_text(stmt, 3);
        const unsigned char *estado = sqlite3_column_text(stmt, 4);
        const unsigned char *motivo = sqlite3_column_text(stmt, 5);

        printf("ID Historial: %d\n", id);
        printf("Paciente ID: %d | Nombre: %s\n", paciente_id, nombre);
        printf("Fecha: %s\n", fecha);
        printf("Estado: %s\n", estado);
        printf("Motivo: %s\n", motivo);
        printf("-------------------------------\n");
    }

    sqlite3_finalize(stmt);
}


void insertarHistorialCita(int paciente_id, int medico_id, const char *fecha, const char *estado, const char *motivo) {
    const char *sql = "INSERT INTO historial_citas (paciente_id, medico_id, fecha, estado, motivo, fecha_modificacion) "
                      "VALUES (?, ?, ?, ?, ?, datetime('now'));";

    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error al preparar la inserción en historial: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, paciente_id);
    sqlite3_bind_int(stmt, 2, medico_id);
    sqlite3_bind_text(stmt, 3, fecha, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, estado, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, motivo, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Registro de historial creado con éxito.\n");
    } else {
        printf("Error al insertar historial: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}