#ifndef LOGS_H
#define LOGS_H

// Registrar un nuevo log en la base de datos
void registrarLog(const char *tipo_evento, const char *descripcion, int usuario_id);

// Mostrar todos los logs almacenados
void listarLogs();

#endif
