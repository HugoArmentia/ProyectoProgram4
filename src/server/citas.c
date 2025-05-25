#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "citas.h"
#include "logs.h"
#include "utils.h"
#include "calendario.h"
#include <stdio.h>
#include <sqlite3.h>
#include "database.h"
#include "historial.h"

#define HORAS_DISPONIBLES 24  
extern const char *horas[];

void listarCitas() {
    const char *sql = "SELECT id, fecha, motivo, dia, mes, anio FROM citas WHERE estado = 'Programada';";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error al preparar la consulta: %s\n", sqlite3_errmsg(db));
        return;
    }

    printf("\n======= CITAS PROGRAMADAS =======\n");

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char *fecha = sqlite3_column_text(stmt, 1);
        const unsigned char *motivo = sqlite3_column_text(stmt, 2);
        int dia = sqlite3_column_int(stmt, 3);
        int mes = sqlite3_column_int(stmt, 4);
        int anio = sqlite3_column_int(stmt, 5);
        printf("ID: %d - Fecha: %d-%d-%d - Hora: %s - Motivo: %s\n", id, dia, mes, anio, fecha, motivo);
    }

    sqlite3_finalize(stmt);
}

void cancelarCita() {
    int citaId;
    printf("Ingrese el ID de la cita que desea cancelar: ");
    scanf("%d", &citaId);
    getchar();

    const char *sql_check = "SELECT estado FROM citas WHERE id = ?;";
    sqlite3_stmt *stmt_check;
    if (sqlite3_prepare_v2(db, sql_check, -1, &stmt_check, NULL) != SQLITE_OK) {
        printf("Error al preparar la consulta de verificación: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt_check, 1, citaId);
    if (sqlite3_step(stmt_check) == SQLITE_ROW) {
        const unsigned char *estado = sqlite3_column_text(stmt_check, 0);
        if (strcmp((const char*)estado, "Programada") != 0) {
            printf("La cita no está programada o ya fue cancelada/completada.\n");
            sqlite3_finalize(stmt_check);
            return;
        }
    } else {
        printf("No se encontró ninguna cita con el ID especificado.\n");
        sqlite3_finalize(stmt_check);
        return;
    }
    sqlite3_finalize(stmt_check);

    const char *sql_update = "UPDATE citas SET estado = 'Cancelada', fecha_modificacion = datetime('now') WHERE id = ?;";
    sqlite3_stmt *stmt_update;
    if (sqlite3_prepare_v2(db, sql_update, -1, &stmt_update, NULL) != SQLITE_OK) {
        printf("Error al preparar la actualización: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt_update, 1, citaId);
    if (sqlite3_step(stmt_update) == SQLITE_DONE) {
        printf("Cita con ID %d ha sido cancelada correctamente.\n", citaId);
        const char *sql_get = "SELECT paciente_id, medico_id, fecha, motivo FROM citas WHERE id = ?;";
        sqlite3_stmt *stmt_get;
        if (sqlite3_prepare_v2(db, sql_get, -1, &stmt_get, NULL) == SQLITE_OK) {
            sqlite3_bind_int(stmt_get, 1, citaId);
            if (sqlite3_step(stmt_get) == SQLITE_ROW) {
                int pacienteId = sqlite3_column_int(stmt_get, 0);
                int medicoId = sqlite3_column_int(stmt_get, 1);
                const char *fecha = (const char *)sqlite3_column_text(stmt_get, 2);
                const char *motivo = (const char *)sqlite3_column_text(stmt_get, 3);
                insertarHistorialCita(pacienteId, medicoId, fecha, "Cancelada", motivo);
            }
            sqlite3_finalize(stmt_get);
        }
    } else {
        printf("Error al cancelar la cita: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt_update);
}

void listarCitasMedico(int medico_id) {
    const char *sql = "SELECT id, paciente_id, fecha, motivo, estado, dia, mes, anio FROM citas "
                      "WHERE medico_id = ? AND estado = 'Programada' ORDER BY anio, mes, dia, fecha;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error al preparar la consulta de citas del medico: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, medico_id);

    int hayResultados = 0;

    printf("\n======= CITAS PROGRAMADAS DEL MEDICO ID %d =======\n", medico_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        hayResultados = 1;

        int id = sqlite3_column_int(stmt, 0);
        int paciente_id = sqlite3_column_int(stmt, 1);
        const unsigned char *hora = sqlite3_column_text(stmt, 2);
        const unsigned char *motivo = sqlite3_column_text(stmt, 3);
        const unsigned char *estado = sqlite3_column_text(stmt, 4);
        int dia = sqlite3_column_int(stmt, 5);
        int mes = sqlite3_column_int(stmt, 6);
        int anio = sqlite3_column_int(stmt, 7);

        printf("ID: %d | Paciente ID: %d | Fecha: %02d-%02d-%d %s | Estado: %s | Motivo: %s\n",
               id, paciente_id, dia, mes, anio, hora, estado, motivo);
    }

    if (!hayResultados) {
        printf("No hay citas programadas actualmente para este medico.\n");
    }

    printf("----------------------------------------------\n");
    sqlite3_finalize(stmt);
}

void modificarCita() {
    int citaId;
    int dia, mes, anio, indiceHora;

    printf("Ingrese el ID de la cita que desea modificar: ");
    scanf("%d", &citaId);
    getchar();

    int medico_id = -1;
    const char *sqlGetMedico = "SELECT medico_id FROM citas WHERE id = ?;";
    sqlite3_stmt *stmtMedico;
    if (sqlite3_prepare_v2(db, sqlGetMedico, -1, &stmtMedico, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmtMedico, 1, citaId);
        if (sqlite3_step(stmtMedico) == SQLITE_ROW) {
            medico_id = sqlite3_column_int(stmtMedico, 0);
        }
        sqlite3_finalize(stmtMedico);
    }

    if (medico_id == -1) {
        printf("No se encontró la cita con ID %d.\n", citaId);
        return;
    }

    printf("Ingrese el nuevo día (1-31): ");
    scanf("%d", &dia); getchar();

    printf("Ingrese el nuevo mes (1-12): ");
    scanf("%d", &mes); getchar();

    printf("Ingrese el nuevo año (ej: 2025): ");
    scanf("%d", &anio); getchar();

    time_t t = time(NULL);
    struct tm *ahora = localtime(&t);
    int diaActual = ahora->tm_mday;
    int mesActual = ahora->tm_mon + 1;
    int anioActual = ahora->tm_year + 1900;

    if (anio < anioActual ||
        (anio == anioActual && mes < mesActual) ||
        (anio == anioActual && mes == mesActual && dia < diaActual)) {
        printf("No puedes modificar la cita a una fecha anterior a la actual.\n");
        return;
    }

    mostrarHorasDisponibles(dia, mes, anio);

    printf("Seleccione un índice de hora (0 - 23): ");
    scanf("%d", &indiceHora); getchar();

    if (indiceHora < 0 || indiceHora >= HORAS_DISPONIBLES) {
        printf("Índice de hora inválido.\n");
        return;
    }

    const char *horaSeleccionada = horas[indiceHora];

    const char *checkSql = "SELECT COUNT(*) FROM citas "
                           "WHERE dia = ? AND mes = ? AND anio = ? AND fecha = ? AND medico_id = ? "
                           "AND estado = 'Programada' AND id != ?;";
    sqlite3_stmt *checkStmt;
    if (sqlite3_prepare_v2(db, checkSql, -1, &checkStmt, NULL) != SQLITE_OK) {
        printf("Error al preparar validación de conflicto: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(checkStmt, 1, dia);
    sqlite3_bind_int(checkStmt, 2, mes);
    sqlite3_bind_int(checkStmt, 3, anio);
    sqlite3_bind_text(checkStmt, 4, horaSeleccionada, -1, SQLITE_STATIC);
    sqlite3_bind_int(checkStmt, 5, medico_id);
    sqlite3_bind_int(checkStmt, 6, citaId);
    int conflicto = 0;
    if (sqlite3_step(checkStmt) == SQLITE_ROW) {
        conflicto = sqlite3_column_int(checkStmt, 0);
    }
    sqlite3_finalize(checkStmt);

    if (conflicto > 0) {
        printf("Ya existe una cita para ese médico a esa hora. No se puede modificar.\n");
        return;
    }

    const char *sqlUpdate = "UPDATE citas SET fecha = ?, dia = ?, mes = ?, anio = ?, fecha_modificacion = datetime('now') WHERE id = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sqlUpdate, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error al preparar actualización de cita: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, horaSeleccionada, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, dia);
    sqlite3_bind_int(stmt, 3, mes);
    sqlite3_bind_int(stmt, 4, anio);
    sqlite3_bind_int(stmt, 5, citaId);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Cita modificada correctamente a %02d-%02d-%d %s\n", dia, mes, anio, horaSeleccionada);
    } else {
        printf("Error al modificar la cita: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}


void actualizarEstadoCita(int medicoId) {
    int citaId;
    char nuevoEstado[20];

    printf("Ingrese el ID de la cita que desea actualizar: ");
    scanf("%d", &citaId);
    getchar();

    printf("Ingrese el nuevo estado (Programada / Completada / Cancelada): ");
    fgets(nuevoEstado, sizeof(nuevoEstado), stdin);
    nuevoEstado[strcspn(nuevoEstado, "\n")] = 0;

    const char *sql_check = "SELECT id FROM citas WHERE id = ? AND medico_id = ?;";
    sqlite3_stmt *stmt_check;

    if (sqlite3_prepare_v2(db, sql_check, -1, &stmt_check, NULL) != SQLITE_OK) {
        printf("Error al preparar la verificación: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt_check, 1, citaId);
    sqlite3_bind_int(stmt_check, 2, medicoId);

    if (sqlite3_step(stmt_check) != SQLITE_ROW) {
        printf("No se encontró una cita con ese ID asignada al médico actual.\n");
        sqlite3_finalize(stmt_check);
        return;
    }
    sqlite3_finalize(stmt_check);

    const char *sql_update = "UPDATE citas SET estado = ?, fecha_modificacion = datetime('now') WHERE id = ?;";
    sqlite3_stmt *stmt_update;

    if (sqlite3_prepare_v2(db, sql_update, -1, &stmt_update, NULL) != SQLITE_OK) {
        printf("Error al preparar la actualización: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt_update, 1, nuevoEstado, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt_update, 2, citaId);

    if (sqlite3_step(stmt_update) == SQLITE_DONE) {
        printf("Estado de la cita actualizado correctamente.\n");

        char descripcion[200];
        sprintf(descripcion, "Cita %d actualizada a estado: %s", citaId, nuevoEstado);
        registrarLog("Actualizar Cita", descripcion, medicoId);

        const char *sql_get = "SELECT paciente_id, medico_id, fecha, motivo FROM citas WHERE id = ?;";
        sqlite3_stmt *stmt_get;
        if (sqlite3_prepare_v2(db, sql_get, -1, &stmt_get, NULL) == SQLITE_OK) {
            sqlite3_bind_int(stmt_get, 1, citaId);
            if (sqlite3_step(stmt_get) == SQLITE_ROW) {
                int pacienteId = sqlite3_column_int(stmt_get, 0);
                int medicoIdFromDb = sqlite3_column_int(stmt_get, 1);
                const char *fecha = (const char *)sqlite3_column_text(stmt_get, 2);
                const char *motivo = (const char *)sqlite3_column_text(stmt_get, 3);

                insertarHistorialCita(pacienteId, medicoIdFromDb, fecha, nuevoEstado, motivo);
            }
            sqlite3_finalize(stmt_get);
        }

    } else {
        printf("Error al actualizar el estado de la cita: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt_update);
}

void listarHistorial() {
    const char *sql = "SELECT id, fecha, estado, motivo, paciente_id, medico_id FROM citas ORDER BY fecha DESC;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error al preparar la consulta del historial: %s\n", sqlite3_errmsg(db));
        return;
    }

    printf("\n======= HISTORIAL DE CITAS =======\n");

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char *fecha = sqlite3_column_text(stmt, 1);
        const unsigned char *estado = sqlite3_column_text(stmt, 2);
        const unsigned char *motivo = sqlite3_column_text(stmt, 3);
        int pacienteId = sqlite3_column_int(stmt, 4);
        int medicoId = sqlite3_column_int(stmt, 5);

        printf("ID: %d | Fecha: %s | Estado: %s | Motivo: %s | Paciente ID: %d | Médico ID: %d\n",
               id, fecha, estado, motivo, pacienteId, medicoId);
    }

    sqlite3_finalize(stmt);
}
