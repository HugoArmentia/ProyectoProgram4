#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "citas.h"
#include "logs.h"
#include "utils.h"
#include "calendario.h"

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
    printf("\n======= CITAS ASIGNADAS =======\n");

    for (int i = 0; i < totalCitas; i++) {
        if (citas[i].medico_id == medicoId) {
            printf("ID Cita: %d\n", citas[i].id);
            printf("Paciente ID: %d\n", citas[i].paciente_id);
            printf("Fecha: %s\n", citas[i].fecha);
            printf("Estado: %s\n", citas[i].estado);
            printf("Motivo: %s\n", citas[i].motivo);
            printf("-------------------------------\n");
        }
    }
}

void modificarCita() {
    int citaId;
    int encontrado = 0;

    printf("Ingrese el ID de la cita que desea modificar: ");
    scanf("%d", &citaId);
    getchar();  // Limpiar el buffer

    for (int i = 0; i < totalCitas; i++) {
        if (citas[i].id == citaId && strcmp(citas[i].estado, "Programada") == 0) {
            encontrado = 1;

            printf("Modificando la cita con ID %d\n", citaId);
            printf("Fecha actual: %d-%d-%d %s\n", citas[i].dia, citas[i].mes, citas[i].anio, citas[i].fecha);
            
            // Solicitar la nueva fecha y hora
            printf("Ingrese el nuevo día (dd): ");
            scanf("%d", &citas[i].dia);
            getchar();  // Limpiar el buffer
            printf("Ingrese el nuevo mes (mm): ");
            scanf("%d", &citas[i].mes);
            getchar();  // Limpiar el buffer
            printf("Ingrese el nuevo año (yyyy): ");
            scanf("%d", &citas[i].anio);
            getchar();  // Limpiar el buffer

            // Solicitar la nueva hora
            printf("Ingrese la nueva hora (HH:MM): ");
            fgets(citas[i].fecha, sizeof(citas[i].fecha), stdin);
            citas[i].fecha[strcspn(citas[i].fecha, "\n")] = '\0';  // Limpiar el salto de línea

            printf("Ingrese el nuevo motivo de la cita: ");
            fgets(citas[i].motivo, sizeof(citas[i].motivo), stdin);
            citas[i].motivo[strcspn(citas[i].motivo, "\n")] = '\0';  // Limpiar el salto de línea

            guardarCitas();
            printf("Cita modificada correctamente.\n");
            return;
        }
    }

    if (!encontrado) {
        printf("No se encontró ninguna cita con el ID especificado o la cita ya ha sido cancelada.\n");
    }
}

void actualizarEstadoCita(int medicoId) { 
    int citaId;
    char nuevoEstado[20];
    printf("Ingrese el ID de la cita que desea actualizar: ");
    scanf("%d", &citaId);
    getchar();

    printf("Ingrese el nuevo estado (Programada / Completada / Cancelada): ");
    fgets(nuevoEstado, 20, stdin);
    nuevoEstado[strcspn(nuevoEstado, "\n")] = '\0';

    for (int i = 0; i < totalCitas; i++) {
        if (citas[i].id == citaId && citas[i].medico_id == medicoId) {
            strcpy(citas[i].estado, nuevoEstado);
            printf("Estado de la cita actualizado correctamente.\n");
            guardarCitas();  // Asegúrate de tener esta función implementada
        }
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
