#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "citas.h"
#include "logs.h"
#include "utils.h"

#define RUTA_CITAS "data/citas.txt"

Cita citas[MAX_CITAS];
int totalCitas = 0;

void cargarCitas() {
    FILE *archivo = fopen(RUTA_CITAS, "r");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo de citas.\n");
        return;
    }

    while (fscanf(archivo, "%d,%d,%d,%19[^,],%19[^,],%99[^\n]\n",
                  &citas[totalCitas].id,
                  &citas[totalCitas].paciente_id,
                  &citas[totalCitas].medico_id,
                  citas[totalCitas].fecha,
                  citas[totalCitas].estado,
                  citas[totalCitas].motivo) == 6) {
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
        fprintf(archivo, "%d,%d,%d,%s,%s,%s\n",
                citas[i].id,
                citas[i].paciente_id,
                citas[i].medico_id,
                citas[i].fecha,
                citas[i].estado,
                citas[i].motivo);
    }

    fclose(archivo);
}

void reservarCita() {
    int paciente_id;
    
    printf("Ingrese su ID de paciente: ");
    scanf("%d", &paciente_id);
    getchar();  // Limpiar el buffer

    if (totalCitas >= MAX_CITAS) {
        printf("Límite máximo de citas alcanzado.\n");
        return;
    }

    Cita nuevaCita;
    nuevaCita.id = totalCitas + 1;
    nuevaCita.paciente_id = paciente_id;

    // Hay que hacer una manera para mirar los medicos disponibles

    printf("Ingrese el ID del médico con el que desea reservar la cita: ");
    scanf("%d", &nuevaCita.medico_id);
    getchar();

    printf("Ingrese la fecha de la cita (YYYY-MM-DD HH:MM): ");
    fgets(nuevaCita.fecha, 20, stdin);
    nuevaCita.fecha[strcspn(nuevaCita.fecha, "\n")] = 0;

    printf("Ingrese el motivo de la cita: ");
    fgets(nuevaCita.motivo, 200, stdin);
    nuevaCita.motivo[strcspn(nuevaCita.motivo, "\n")] = 0;

    strcpy(nuevaCita.estado, "Programada");

    citas[totalCitas] = nuevaCita;
    totalCitas++;

    guardarCitas();

    printf("Cita reservada correctamente.\n");
}

void listarCitas() {
    for (int i = 0; i < totalCitas; i++) {
        printf("Cita ID: %d - Motivo: %s\n", citas[i].id, citas[i].motivo);
    }
}

void cancelarCita() {
    int citaId;
    int encontrado = 0;

    printf("Ingrese el ID de la cita que desea cancelar: ");
    scanf("%d", &citaId);
    getchar();  // Limpiar el buffer

    for (int i = 0; i < totalCitas; i++) {
        if (citas[i].id == citaId) {
            encontrado = 1;

            if (strcmp(citas[i].estado, "Cancelada") == 0) {
                printf("Esta cita ya ha sido cancelada.\n");
                return;
            }

            // Cambiar el estado de la cita a Cancelada
            strcpy(citas[i].estado, "Cancelada");
            guardarCitas();

            printf("Cita con ID %d ha sido cancelada correctamente.\n", citaId);
            return;
        }
    }

    if (!encontrado) {
        printf("No se encontró ninguna cita con el ID especificado.\n");
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
        if (citas[i].id == citaId) {
            encontrado = 1;

            printf("Modificando la fecha de la cita con ID %d\n", citaId);
            printf("Fecha actual: %s\n", citas[i].fecha);
            printf("Ingrese la nueva fecha (YYYY-MM-DD HH:MM): ");
            
            char nuevaFecha[20];
            fgets(nuevaFecha, 20, stdin);
            nuevaFecha[strcspn(nuevaFecha, "\n")] = 0;  // Eliminar el salto de línea

            if (strlen(nuevaFecha) > 0) {
                strcpy(citas[i].fecha, nuevaFecha);
                guardarCitas();
                printf("Fecha modificada correctamente.\n");
            } else {
                printf("No se ingresó una nueva fecha. No se realizaron cambios.\n");
            }
            return;
        }
    }

    if (!encontrado) {
        printf("No se encontró ninguna cita con el ID especificado.\n");
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