#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

#define MAX_CONFIG_LINE 200

Config configuracion;  // Variable global que contiene la configuración actual

// Cargar la configuración desde un archivo de texto
void cargarConfiguracion(const char *rutaArchivo) {
    FILE *archivo = fopen(rutaArchivo, "r");
    if (archivo == NULL) {
        printf("Error al abrir el archivo de configuración: %s\n", rutaArchivo);
        return;
    }

    char linea[MAX_CONFIG_LINE];
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        char clave[50], valor[100];
        if (sscanf(linea, "%[^=]=%s", clave, valor) == 2) {
            if (strcmp(clave, "nombre_base_datos") == 0) {
                strcpy(configuracion.nombre_base_datos, valor);
            } else if (strcmp(clave, "puerto_servidor") == 0) {
                configuracion.puerto_servidor = atoi(valor);
            } else if (strcmp(clave, "log_path") == 0) {
                strcpy(configuracion.log_path, valor);
            } else if (strcmp(clave, "max_conexiones") == 0) {
                configuracion.max_conexiones = atoi(valor);
            } else if (strcmp(clave, "modo_debug") == 0) {
                configuracion.modo_debug = atoi(valor);
            }
        }
    }

    fclose(archivo);
    printf("Configuración cargada correctamente.\n");
}

// Guardar la configuración en un archivo de texto
void guardarConfiguracion(const char *rutaArchivo) {
    FILE *archivo = fopen(rutaArchivo, "w");
    if (archivo == NULL) {
        printf("Error al guardar el archivo de configuración: %s\n", rutaArchivo);
        return;
    }

    fprintf(archivo, "nombre_base_datos=%s\n", configuracion.nombre_base_datos);
    fprintf(archivo, "puerto_servidor=%d\n", configuracion.puerto_servidor);
    fprintf(archivo, "log_path=%s\n", configuracion.log_path);
    fprintf(archivo, "max_conexiones=%d\n", configuracion.max_conexiones);
    fprintf(archivo, "modo_debug=%d\n", configuracion.modo_debug);

    fclose(archivo);
    printf("Configuración guardada correctamente.\n");
}
