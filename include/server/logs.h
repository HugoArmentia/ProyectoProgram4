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

extern LogActividad logs[MAX_LOGS]; 
extern int totalLogs;              

void cargarLogs();                
void guardarLogs();                  
void registrarLog(const char *tipo_evento, const char *descripcion, int usuario_id); 
void listarLogs();               

#endif
