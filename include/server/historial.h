#ifndef HISTORIAL_H
#define HISTORIAL_H

void listarHistorialMedico(int medicoId);
void insertarHistorialCita(int paciente_id, int medico_id, const char *fecha, const char *estado, const char *motivo);

#endif
