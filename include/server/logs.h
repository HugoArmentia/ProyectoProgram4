#ifndef LOGS_H
#define LOGS_H

#define MAX_LOGS 1000
#define MAX_LOG_DESC 200
#define MAX_LOG_TIPO 50

typedef struct {
    int id;
    char tipo_evento[MAX_LOG_TIPO];
    char descripcion[MAX_LOG_DESC];
    int usuario_id;
    char fecha_evento[20];
} LogActividad;

// Declaraciones globales para acceso a logs
extern LogActividad logs[MAX_LOGS];  // Array de logs
extern int totalLogs;                 // Número total de logs registrados

// Funciones para manipulación de logs
void cargarLogs();                    // Cargar logs desde archivo
void guardarLogs();                   // Guardar logs en archivo
void registrarLog(const char *tipo_evento, const char *descripcion, int usuario_id); // Añadir un nuevo log
void listarLogs();                     // Mostrar todos los logs en consola

#endif
