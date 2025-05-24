#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

#define MAX_CONFIG_LINE 200

Config configuracion;

int cargarConfiguracion(const char *rutaArchivo) {
    FILE *archivo = fopen(rutaArchivo, "r");
    if (archivo == NULL) {
        printf("❌ Error al abrir el archivo de configuración: %s\n", rutaArchivo);
        return 0;
    }

    char linea[MAX_CONFIG_LINE];
    while (fgets(linea, sizeof(linea), archivo)) {
        char clave[50], valor[100];
        if (sscanf(linea, "%49[^=]=%99[^\n]", clave, valor) == 2) {
            if (strcmp(clave, "nombre_base_datos") == 0) {
                strncpy(configuracion.nombre_base_datos, valor, sizeof(configuracion.nombre_base_datos) - 1);
            } else if (strcmp(clave, "puerto_servidor") == 0) {
                configuracion.puerto_servidor = atoi(valor);
            } else if (strcmp(clave, "log_path") == 0) {
                strncpy(configuracion.log_path, valor, sizeof(configuracion.log_path) - 1);
            } else if (strcmp(clave, "max_conexiones") == 0) {
                configuracion.max_conexiones = atoi(valor);
            } else if (strcmp(clave, "modo_debug") == 0) {
                configuracion.modo_debug = atoi(valor);
            } else {
                printf("⚠️ Clave desconocida en config: %s\n", clave);
            }
        }
    }

    fclose(archivo);

    // Validaciones mínimas
    if (strlen(configuracion.nombre_base_datos) == 0 || configuracion.puerto_servidor == 0) {
        printf("❌ Error: configuración incompleta (nombre_base_datos o puerto no válidos).\n");
        return 0;
    }

    printf("✅ Configuración cargada correctamente: Puerto=%d | DB=%s\n",
           configuracion.puerto_servidor, configuracion.nombre_base_datos);

    return 1;
}

void guardarConfiguracion(const char *rutaArchivo) {
    FILE *archivo = fopen(rutaArchivo, "w");
    if (archivo == NULL) {
        printf("❌ Error al guardar el archivo de configuración: %s\n", rutaArchivo);
        return;
    }

    fprintf(archivo, "nombre_base_datos=%s\n", configuracion.nombre_base_datos);
    fprintf(archivo, "puerto_servidor=%d\n", configuracion.puerto_servidor);
    fprintf(archivo, "log_path=%s\n", configuracion.log_path);
    fprintf(archivo, "max_conexiones=%d\n", configuracion.max_conexiones);
    fprintf(archivo, "modo_debug=%d\n", configuracion.modo_debug);

    fclose(archivo);
    printf("💾 Configuración guardada correctamente.\n");
}
