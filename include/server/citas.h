#ifndef CITAS_H
#define CITAS_H

#define MAX_CITAS 1000

typedef struct {
    int id;
    int paciente_id;
    int medico_id;
    int dia;
    int mes;
    int anio;
    char fecha[20];       // Formato: "YYYY-MM-DD HH:MM:SS"
    char estado[20];       // Ej: "Programada" o "Cancelada"
    char motivo[100];      // Motivo de la cita
} Cita;

extern Cita citas[MAX_CITAS];
extern int totalCitas;

// Funciones para manejo de citas
void cargarCitas();
void guardarCitas();
void listarCitas();
void modificarCita();
void cancelarCita();
void listarHistorial();
void listarCitasMedico(int medicoId);
void actualizarEstadoCita(int medicoId);

#endif
