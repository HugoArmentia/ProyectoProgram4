#ifndef CONFIG_H
#define CONFIG_H

#define MAX_CONFIG_LINE 200

typedef struct {
    char nombre_base_datos[50];
    int puerto_servidor;
    char log_path[100];
    int max_conexiones;
    int modo_debug;
} Config;

extern Config configuracion;

int cargarConfiguracion(const char *rutaArchivo);
void guardarConfiguracion(const char *rutaArchivo);

#endif
