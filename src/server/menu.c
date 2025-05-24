#include <stdio.h>
#include <string.h>
#include "menu.h"
#include "usuarios.h"
#include "citas.h"
#include "historial.h"
#include "logs.h"
#include "calendario.h"

extern int usuarioActualId;

void mostrarMenuPaciente() {
    int opcion;
    int paciente_id = usuarioActualId;

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
                registrarLog("Paciente", "Reservó una cita", paciente_id);
                reservarCitaDesdeCalendario(paciente_id);
                break;
            case 2:
                registrarLog("Paciente", "Consultó sus citas programadas", paciente_id);
                listarCitas();
                break;
            case 3:
                registrarLog("Paciente", "Modificó una cita", paciente_id);
                modificarCita();
                break;
            case 4:
                registrarLog("Paciente", "Canceló una cita", paciente_id);
                cancelarCita();
                break;
            case 5:
                registrarLog("Paciente", "Consultó su historial de citas", paciente_id);
                listarHistorial();
                break;
            case 0:
                registrarLog("Paciente", "Cerró sesión", paciente_id);
                printf("Cerrando sesión de Paciente...\n");
                return;
            default:
                registrarLog("Paciente", "Seleccionó una opción inválida", paciente_id);
                printf("Opción no válida. Intente nuevamente.\n");
        }
    } while(opcion != 0);
}

void mostrarMenuMedico() {
    int opcion;
    int medicoId = usuarioActualId;

    do {
        printf("\n======= MENU MEDICO =======\n");
        printf("1. Consultar citas asignadas\n");
        printf("2. Ver historial de citas\n");
        printf("3. Actualizar estado de una cita\n");
        printf("0. Cerrar sesión\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        getchar();

        switch(opcion) {
            case 1:
                registrarLog("Medico", "Consultó sus citas asignadas", medicoId);
                listarCitasMedico(medicoId);
                break;
            case 2:
                registrarLog("Medico", "Consultó su historial de citas", medicoId);
                listarHistorialMedico(medicoId);
                break;
            case 3:
                registrarLog("Medico", "Actualizó el estado de una cita", medicoId);
                actualizarEstadoCita(medicoId);
                break;
            case 0:
                registrarLog("Medico", "Cerró sesión", medicoId);
                printf("Cerrando sesión de Medico...\n");
                return;
            default:
                registrarLog("Medico", "Seleccionó una opción inválida", medicoId);
                printf("Opción no válida. Intente nuevamente.\n");
        }
    } while(opcion != 0);
}

void mostrarMenuAdmin() {
    int opcion;

    do {
        printf("\n======= MENU ADMINISTRADOR =======\n");
        printf("1. Listar Usuarios\n");
        printf("2. Ver Logs\n");
        printf("3. Crear Usuario Nuevo\n");
        printf("4. Modificar Usuario\n");
        printf("5. Eliminar Usuario\n");
        printf("0. Cerrar sesión\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        getchar();

        switch(opcion) {
            case 1:
                registrarLog("Admin", "Listó todos los usuarios", usuarioActualId);
                listarUsuarios();
                break;
            case 2:
                registrarLog("Admin", "Consultó los logs del sistema", usuarioActualId);
                listarLogs();
                break;
            case 3:
                registrarUsuario();
                registrarLog("Admin", "Registró un nuevo usuario", usuarioActualId);
                break;
            case 4:
                modificarUsuario();
                registrarLog("Admin", "Modificó un usuario", usuarioActualId);
                break;
            case 5:
                eliminarUsuario();
                registrarLog("Admin", "Eliminó un usuario", usuarioActualId);
                break;
            case 0:
                registrarLog("Admin", "Cerró sesión", usuarioActualId);
                printf("Cerrando sesión de Administrador...\n");
                return;
            default:
                registrarLog("Admin", "Seleccionó una opción no válida", usuarioActualId);
                printf("Opción no válida. Intente nuevamente.\n");
        }
    } while(opcion != 0);
}