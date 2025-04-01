#ifndef CONFIG_H
#define CONFIG_H

#define MAX_CONFIG_LINE 200  // Longitud máxima de cada línea de configuración

// Estructura que almacena la configuración del sistema
typedef struct {
    char nombre_base_datos[50];
    int puerto_servidor;
    char log_path[100];
    int max_conexiones;
    int modo_debug;
} Config;

// Variable global que almacenará la configuración actual
extern Config configuracion;

// Funciones para manejar la configuración
void cargarConfiguracion(const char *rutaArchivo);
void guardarConfiguracion(const char *rutaArchivo);

#endif
