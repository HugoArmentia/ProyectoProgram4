#ifndef LOGS_H
#define LOGS_H

void registrarLog(const char *tipo_evento, const char *descripcion, int usuario_id);

void listarLogs();

#endif
