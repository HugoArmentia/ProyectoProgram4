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

#define RUTA_CITAS "data/citas.txt"

Cita citas[MAX_CITAS];
int totalCitas = 0;

void cargarCitas() {
    FILE *archivo = fopen(RUTA_CITAS, "r");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo de citas.\n");
        return;
    }

    while (fscanf(archivo, "%d,%d,%d,%d,%d,%d,%19[^,],%19[^,],%99[^\n]\n",
                  &citas[totalCitas].id,
                  &citas[totalCitas].paciente_id,
                  &citas[totalCitas].medico_id,
                  &citas[totalCitas].dia,
                  &citas[totalCitas].mes,
                  &citas[totalCitas].anio,
                  citas[totalCitas].fecha,
                  citas[totalCitas].estado,
                  citas[totalCitas].motivo) == 9) {
        totalCitas++;
    }

    fclose(archivo);
}

void guardarCitas() {
    FILE *archivo = fopen(RUTA_CITAS, "w");
    if (archivo == NULL) {
        printf("Error al guardar el archivo de citas.\n");
        return;
    }

    for (int i = 0; i < totalCitas; i++) {
        fprintf(archivo, "%d,%d,%d,%d,%d,%d,%s,%s,%s\n",
                citas[i].id,
                citas[i].paciente_id,
                citas[i].medico_id,
                citas[i].dia,
                citas[i].mes,
                citas[i].anio,
                citas[i].fecha,
                citas[i].estado,
                citas[i].motivo);
    }

    fclose(archivo);
}

void listarCitas() {
    printf("\n======= CITAS PROGRAMADAS =======\n");

    // Recorrer todas las citas y mostrar las programadas
    for (int i = 0; i < totalCitas; i++) {
        if (strcmp(citas[i].estado, "Programada") == 0) {
            printf("ID: %d - Fecha: %d-%d-%d %s - Motivo: %s\n", 
                    citas[i].id, 
                    citas[i].dia, 
                    citas[i].mes, 
                    citas[i].anio, 
                    citas[i].fecha, 
                    citas[i].motivo);
        }
    }
}

void cancelarCita() {
    int citaId;
    int encontrado = 0;

    printf("Ingrese el ID de la cita que desea cancelar: ");
    scanf("%d", &citaId);
    getchar();  // Limpiar el buffer

    for (int i = 0; i < totalCitas; i++) {
        if (citas[i].id == citaId && strcmp(citas[i].estado, "Programada") == 0) {
            encontrado = 1;

            // Cambiar el estado de la cita a Cancelada
            strcpy(citas[i].estado, "Cancelada");
            guardarCitas();

            printf("Cita con ID %d ha sido cancelada correctamente.\n", citaId);
            return;
        }
    }

    if (!encontrado) {
        printf("No se encontró ninguna cita con el ID especificado o la cita ya está cancelada.\n");
    }
}



void listarCitasMedico(int medicoId) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, paciente_id, fecha, estado, motivo FROM citas WHERE medico_id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, medicoId);

        printf("======= CITAS PROGRAMADAS =======\n");
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int citaId = sqlite3_column_int(stmt, 0);
            int pacienteId = sqlite3_column_int(stmt, 1);
            const char *fecha = (const char*)sqlite3_column_text(stmt, 2);
            const char *estado = (const char*)sqlite3_column_text(stmt, 3);
            const char *motivo = (const char*)sqlite3_column_text(stmt, 4);

            printf("Cita ID: %d\n", citaId);
            printf("Paciente ID: %d\n", pacienteId);
            printf("Fecha: %s\n", fecha);
            printf("Estado: %s\n", estado);
            printf("Motivo: %s\n", motivo);
            printf("-------------------------\n");
        }
        sqlite3_finalize(stmt);
    } else {
        printf("Error al obtener las citas: %s\n", sqlite3_errmsg(db));
    }
}

<<<<<<< HEAD
void modificarCita() {
=======



void modificarCitaMedico(int medicoId) {
>>>>>>> 52d09520335cb9c097cbcb8da76e45d52463ad26
    int citaId;
    char nuevaFecha[20];

    printf("Ingrese el ID de la cita que desea modificar: ");
    scanf("%d", &citaId);
    getchar();

    printf("Ingrese la nueva fecha para la cita (YYYY-MM-DD HH:MM): ");
    fgets(nuevaFecha, sizeof(nuevaFecha), stdin);
    nuevaFecha[strcspn(nuevaFecha, "\n")] = 0;

    sqlite3_stmt *stmt;
    const char *sql = "UPDATE citas SET fecha = ? WHERE id = ? AND medico_id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, nuevaFecha, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, citaId);
        sqlite3_bind_int(stmt, 3, medicoId);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            printf("Cita modificada correctamente.\n");
        } else {
            printf("Error al modificar la cita: %s\n", sqlite3_errmsg(db));
        }
        sqlite3_finalize(stmt);
    } else {
        printf("Error al preparar la consulta: %s\n", sqlite3_errmsg(db));
    }
}

<<<<<<< HEAD
void actualizarEstadoCita(int medicoId) { 
    int citaId;
    char nuevoEstado[20];
    int citaEncontrada = 0;  // Declara citaEncontrada como un entero

    printf("Ingrese el ID de la cita que desea actualizar: ");
=======


void cancelarCitaMedico(int medicoId) {
    int citaId;

    printf("Ingrese el ID de la cita que desea cancelar: ");
>>>>>>> 52d09520335cb9c097cbcb8da76e45d52463ad26
    scanf("%d", &citaId);
    getchar();  // Limpiar el buffer

<<<<<<< HEAD
    printf("Ingrese el nuevo estado (Programada / Completada / Cancelada): ");
    fgets(nuevoEstado, 20, stdin);
    nuevoEstado[strcspn(nuevoEstado, "\n")] = '\0';  // Eliminar salto de línea

    for (int i = 0; i < totalCitas; i++) {
        if (citas[i].id == citaId && citas[i].medico_id == medicoId) {
            strcpy(citas[i].estado, nuevoEstado);
            printf("Estado de la cita actualizado correctamente.\n");

            guardarCitas();

            // Registrar el cambio en el log
            char descripcion[200];
            sprintf(descripcion, "Cita %d actualizada a estado: %s", citaId, nuevoEstado);
            registrarLog("Actualizar Cita", descripcion, medicoId);

            citaEncontrada = 1;  // Ahora citaEncontrada está declarada correctamente
            break;
=======
    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM citas WHERE id = ? AND medico_id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, citaId);
        sqlite3_bind_int(stmt, 2, medicoId);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            printf("Cita cancelada correctamente.\n");
        } else {
            printf("Error al cancelar la cita: %s\n", sqlite3_errmsg(db));
>>>>>>> 52d09520335cb9c097cbcb8da76e45d52463ad26
        }
        sqlite3_finalize(stmt);
    } else {
        printf("Error al preparar la consulta: %s\n", sqlite3_errmsg(db));
    }

    // Si no se encontró la cita
    if (!citaEncontrada) {
        printf("No se encontró ninguna cita con el ID especificado.\n");
    }
}

void listarHistorial() {
    printf("\n======= HISTORIAL DE CITAS =======\n");

    // Recorrer todas las citas y mostrar todas
    for (int i = 0; i < totalCitas; i++) {
        printf("ID: %d - Fecha: %d-%d-%d %s - Estado: %s - Motivo: %s\n", 
                citas[i].id, 
                citas[i].dia, 
                citas[i].mes, 
                citas[i].anio, 
                citas[i].fecha, 
                citas[i].estado, 
                citas[i].motivo);
    }
}


void listarHistorialMedico(int medicoId) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, paciente_id, fecha, estado, motivo FROM historial_citas WHERE medico_id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, medicoId);

        printf("======= HISTORIAL DE CITAS =======\n");
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int citaId = sqlite3_column_int(stmt, 0);
            int pacienteId = sqlite3_column_int(stmt, 1);
            const char *fecha = (const char*)sqlite3_column_text(stmt, 2);
            const char *estado = (const char*)sqlite3_column_text(stmt, 3);
            const char *motivo = (const char*)sqlite3_column_text(stmt, 4);

            printf("Cita ID: %d\n", citaId);
            printf("Paciente ID: %d\n", pacienteId);
            printf("Fecha: %s\n", fecha);
            printf("Estado: %s\n", estado);
            printf("Motivo: %s\n", motivo);
            printf("-------------------------\n");
        }
        sqlite3_finalize(stmt);
    } else {
        printf("Error al obtener el historial: %s\n", sqlite3_errmsg(db));
    }
}

