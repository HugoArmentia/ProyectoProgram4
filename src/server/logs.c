#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logs.h"
#include "utils.h"

LogActividad logs[MAX_LOGS];
int totalLogs = 0;

void cargarLogs() {
    FILE *archivo = fopen("data/logs_actividades.txt", "r");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo de logs.\n");
        return;
    }

    totalLogs = 0;
    while (fscanf(archivo, "%d,%49[^,],%199[^,],%d,%19s\n",
                  &logs[totalLogs].id,
                  logs[totalLogs].tipo_evento,
                  logs[totalLogs].descripcion,
                  &logs[totalLogs].usuario_id,
                  logs[totalLogs].fecha_evento) == 5) {
        totalLogs++;
        if (totalLogs >= MAX_LOGS) break;
    }

    fclose(archivo);
}

void guardarLogs() {
    FILE *archivo = fopen("data/logs_actividades.txt", "w");
    if (archivo == NULL) {
        printf("Error al guardar el archivo de logs.\n");
        return;
    }
    // estamos cooked
    for (int i = 0; i < totalLogs; i++) {
        fprintf(archivo, "%d,%s,%s,%d,%s\n",
                logs[i].id,
                logs[i].tipo_evento,
                logs[i].descripcion,
                logs[i].usuario_id,
                logs[i].fecha_evento);
    }

    fclose(archivo);
}

void registrarLog(const char *tipo_evento, const char *descripcion, int usuario_id) {
    if (totalLogs >= MAX_LOGS) {
        printf("Se alcanzó el límite máximo de logs registrados.\n");
        return;
    }

    LogActividad nuevoLog;
    nuevoLog.id = totalLogs + 1;
    strncpy(nuevoLog.tipo_evento, tipo_evento, MAX_LOG_TIPO - 1);
    nuevoLog.tipo_evento[MAX_LOG_TIPO - 1] = '\0';

    strncpy(nuevoLog.descripcion, descripcion, MAX_LOG_DESC - 1);
    nuevoLog.descripcion[MAX_LOG_DESC - 1] = '\0';

    nuevoLog.usuario_id = usuario_id;

    obtenerFechaHoraActual(nuevoLog.fecha_evento);

    logs[totalLogs] = nuevoLog;
    totalLogs++;

    guardarLogs();
}

void listarLogs() {
    printf("======= LISTADO DE LOGS =======\n");

    for (int i = 0; i < totalLogs; i++) {
        printf("ID: %d\n", logs[i].id);
        printf("Tipo de Evento: %s\n", logs[i].tipo_evento);
        printf("Descripción: %s\n", logs[i].descripcion);
        printf("ID de Usuario: %d\n", logs[i].usuario_id);
        printf("Fecha del Evento: %s\n", logs[i].fecha_evento);
        printf("------------------------------\n");
    }

    if (totalLogs == 0) {
        printf("No hay logs registrados.\n");
    }
}
