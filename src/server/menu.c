#include <stdio.h>
#include <string.h>
#include "menu.h"
#include "usuarios.h"
#include "citas.h"
#include "historial.h"
#include "logs.h"

// Menú exclusivo para Pacientes
void mostrarMenuPaciente() {
    int opcion;

    do {
        printf("\n======= MENU PACIENTE =======\n");
        printf("1. Reservar cita\n");
        printf("2. Consultar citas programadas\n");
        printf("3. Modificar cita\n");
        printf("4. Cancelar cita\n");
        printf("5. Ver historial de citas\n");
        printf("0. Cerrar sesión\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        getchar();

        switch(opcion) {
            case 1:
                crearCita();  // Implementar esta función
                break;
            case 2:
                listarCitas();  // Implementar esta función
                break;
            case 4:
                cancelarCita();  // Implementar esta función
                break;
            case 5:
                listarHistorial();  // Implementar esta función
                break;
            case 0:
                printf("Cerrando sesión de Paciente...\n");
                return;
            default:
                printf("Opción no válida. Intente nuevamente.\n");
        }
    } while(opcion != 0);
}

// menu medicos
void mostrarMenuMedico(int medicoId) {  
    int opcion;

    do {
        printf("\n======= MENU MEDICO =======\n");
        printf("1. Consultar citas asignadas\n");
        printf("2. Ver historial de citas\n");
        printf("3. Actualizar estado de una cita\n");
        printf("0. Cerrar sesión\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        getchar();  // Limpiar el buffer

        switch(opcion) {
            case 1:
                listarCitasMedico(medicoId);  
                break;
            case 2:
                listarHistorialMedico(medicoId);  
                break;
            case 3:
                actualizarEstadoCita(medicoId);  
                break;
            case 0:
                printf("Cerrando sesión de Médico...\n");
                return;
            default:
                printf("Opción no válida. Intente nuevamente.\n");
        }
    } while(opcion != 0);
}

// Menú exclusivo para Administradores
void mostrarMenuAdmin() {
    int opcion;

    do {
        printf("\n======= MENU ADMINISTRADOR =======\n");
        printf("1. Listar Usuarios\n");
        printf("2. Ver Logs\n");
        printf("3. Crear Usuario Nuevo\n");
        printf("0. Cerrar sesión\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        getchar();

        switch(opcion) {
            case 1:
                listarUsuarios();
                break;
            case 2:
                listarLogs();
                break;
            case 3:
                registrarUsuario();
                break;
            case 0:
                printf("Cerrando sesión de Administrador...\n");
                return;
            default:
                printf("Opción no válida. Intente nuevamente.\n");
        }
    } while(opcion != 0);
}
