#ifndef HISTORIAL_H
#define HISTORIAL_H

#define MAX_HISTORIAL 1000

typedef struct {
    int id;
    int paciente_id;
    int medico_id;
    char fecha[20];
    char estado[20];
    char motivo[100];
    char fecha_modificacion[20];
} HistorialCita;

extern HistorialCita historial[MAX_HISTORIAL];
extern int totalHistorial;

void cargarHistorial();
void guardarHistorial();
void registrarEnHistorial(HistorialCita nuevaEntrada);
void listarHistorial();
void listarHistorialMedico(int medicoId);


#endif
