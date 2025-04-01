#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "historial.h"
#include "utils.h"

#define RUTA_HISTORIAL "data/historial_citas.txt"

HistorialCita historial[MAX_HISTORIAL];
int totalHistorial = 0;

void cargarHistorial() {
    FILE *archivo = fopen(RUTA_HISTORIAL, "r");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo de historial.\n");
        return;
    }

    while (fscanf(archivo, "%d,%d,%d,%19[^,],%19[^,],%99[^,],%19s\n",
                  &historial[totalHistorial].id,
                  &historial[totalHistorial].paciente_id,
                  &historial[totalHistorial].medico_id,
                  historial[totalHistorial].fecha,
                  historial[totalHistorial].estado,
                  historial[totalHistorial].motivo,
                  historial[totalHistorial].fecha_modificacion) == 7) {
        totalHistorial++;
    }

    fclose(archivo);
}

void guardarHistorial() {
    FILE *archivo = fopen(RUTA_HISTORIAL, "w");
    if (archivo == NULL) {
        printf("Error al guardar el archivo de historial.\n");
        return;
    }

    for (int i = 0; i < totalHistorial; i++) {
        fprintf(archivo, "%d,%d,%d,%s,%s,%s,%s\n",
                historial[i].id,
                historial[i].paciente_id,
                historial[i].medico_id,
                historial[i].fecha,
                historial[i].estado,
                historial[i].motivo,
                historial[i].fecha_modificacion);
    }

    fclose(archivo);
}

void listarHistorial() {
    if (totalHistorial == 0) {
        printf("No hay historial de citas registrado.\n");
        return;
    }

    printf("======= LISTADO DE HISTORIAL DE CITAS =======\n");

    for (int i = 0; i < totalHistorial; i++) {
        printf("ID de Historial: %d\n", historial[i].id);
        printf("ID del Paciente: %d\n", historial[i].paciente_id);
        printf("ID del Médico: %d\n", historial[i].medico_id);
        printf("Fecha: %s\n", historial[i].fecha);
        printf("Estado: %s\n", historial[i].estado);
        printf("Motivo: %s\n", historial[i].motivo);
        printf("---------------------------------------------\n");
    }
}

void listarHistorialMedico(int medicoId) {
    printf("\n======= HISTORIAL DE CITAS =======\n");

    for (int i = 0; i < totalHistorial; i++) {
        if (historial[i].medico_id == medicoId) {
            printf("ID Historial: %d\n", historial[i].id);
            printf("Paciente ID: %d\n", historial[i].paciente_id);
            printf("Fecha: %s\n", historial[i].fecha);
            printf("Estado: %s\n", historial[i].estado);
            printf("Motivo: %s\n", historial[i].motivo);
            printf("-------------------------------\n");
        }
    }
}
