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

void crearCita() {
    if (totalCitas >= MAX_CITAS) {
        printf("Límite máximo de citas alcanzado.\n");
        return;
    }

    Cita nuevaCita;
    nuevaCita.id = totalCitas + 1;

    printf("Ingrese el motivo de la cita: ");
    fgets(nuevaCita.motivo, 200, stdin);
    nuevaCita.motivo[strcspn(nuevaCita.motivo, "\n")] = 0;

    citas[totalCitas] = nuevaCita;
    totalCitas++;
    printf("Cita creada correctamente.\n");
}

void listarCitas() {
    for (int i = 0; i < totalCitas; i++) {
        printf("Cita ID: %d - Motivo: %s\n", citas[i].id, citas[i].motivo);
    }
}

void cancelarCita(int id) {
    if (id <= 0 || id > totalCitas) {
        printf("ID de cita inválido.\n");
        return;
    }
    printf("Cita con ID %d cancelada correctamente.\n", id);
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

void actualizarEstadoCita(int medicoId) { 
    int citaId;
    char nuevoEstado[20];
    int citaEncontrada = 0;

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

          
            guardarCitas();

            
            char descripcion[200];
            sprintf(descripcion, "Cita %d actualizada a estado: %s", citaId, nuevoEstado);
            registrarLog("Actualizar Cita", descripcion, medicoId);

            citaEncontrada = 1;
            break;
        }
    }

    if (!citaEncontrada) {
        printf("No se encontró la cita o no pertenece a este médico.\n");
    }
}
