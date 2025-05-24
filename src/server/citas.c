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
extern const char *horas[];  // declaramos el array de horas si está definido en otro archivo

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
    const char *sql = "SELECT id, paciente_id, fecha, motivo, estado FROM citas WHERE medico_id = ? ORDER BY fecha;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error al preparar la consulta de citas del médico: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, medico_id);

    printf("\n======= CITAS ASIGNADAS AL MÉDICO ID %d =======\n", medico_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int paciente_id = sqlite3_column_int(stmt, 1);
        const unsigned char *fecha = sqlite3_column_text(stmt, 2);
        const unsigned char *motivo = sqlite3_column_text(stmt, 3);
        const unsigned char *estado = sqlite3_column_text(stmt, 4);

        printf("ID: %d | Paciente ID: %d | Fecha: %s | Estado: %s | Motivo: %s\n",
               id, paciente_id, fecha, estado, motivo);
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

    printf("Ingrese el nuevo día (1-31): ");
    scanf("%d", &dia);
    getchar();

    printf("Ingrese el nuevo mes (1-12): ");
    scanf("%d", &mes);
    getchar();

    printf("Ingrese el nuevo año (ej: 2025): ");
    scanf("%d", &anio);
    getchar();

    mostrarHorasDisponibles(dia, mes, anio);

    printf("Seleccione un índice de hora (0 - 23): ");
    scanf("%d", &indiceHora);
    getchar();

    if (indiceHora < 0 || indiceHora >= HORAS_DISPONIBLES) {
        printf("Índice de hora inválido.\n");
        return;
    }

    const char *horaSeleccionada = horas[indiceHora];

    const char *sql = "UPDATE citas SET fecha = ?, dia = ?, mes = ?, anio = ?, fecha_modificacion = datetime('now') WHERE id = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error al preparar la consulta de actualización: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, horaSeleccionada, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, dia);
    sqlite3_bind_int(stmt, 3, mes);
    sqlite3_bind_int(stmt, 4, anio);
    sqlite3_bind_int(stmt, 5, citaId);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Cita modificada correctamente a %d-%d-%d %s.\n", dia, mes, anio, horaSeleccionada);
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
    getchar();  // Limpiar buffer

    printf("Ingrese el nuevo estado (Programada / Completada / Cancelada): ");
    fgets(nuevoEstado, sizeof(nuevoEstado), stdin);
    nuevoEstado[strcspn(nuevoEstado, "\n")] = 0;

    // Verificar si la cita pertenece al médico
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

    // Actualizar estado
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

        // Registrar en log
        char descripcion[200];
        sprintf(descripcion, "Cita %d actualizada a estado: %s", citaId, nuevoEstado);
        registrarLog("Actualizar Cita", descripcion, medicoId);

        // Obtener información y registrar en historial
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
