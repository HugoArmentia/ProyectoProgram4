#include <stdio.h>
#include <string.h>
#include "menu.h"
#include "usuarios.h"
#include "citas.h"
#include "historial.h"
#include "logs.h"

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
                reservarCita();
                break;
            case 2:
                listarCitas();
                break;
            case 3:
                modificarCita();
                break;
            case 4:
                cancelarCita();
                break;
            case 5:
                listarHistorial();
                break;
            case 0:
                printf("Cerrando sesión de Paciente...\n");
                return;
            default:
                printf("Opción no válida. Intente nuevamente.\n");
        }
    } while(opcion != 0);
}

void mostrarMenuMedico() {
    int opcion;

    do {
        printf("\n======= MENU MEDICO =======\n");
        printf("1. Consultar citas asignadas\n");
        printf("2. Ver historial de citas\n");
        printf("0. Cerrar sesión\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        getchar();

        switch(opcion) {
            case 1:
<<<<<<< HEAD
                listarCitasMedico(medicoId);  
                break;
            case 2:
                listarHistorialMedico(medicoId);  
                break;
            case 3:
                actualizarEstadoCita(medicoId);  
=======
                listarCitas();
                break;
            case 2:
                listarHistorial();
>>>>>>> 1a72b8dbf1fe41fb65028626b4875b0996d9efcf
                break;
            case 0:
                printf("Cerrando sesión de Medico...\n");
                return;
            default:
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
                listarUsuarios();
                break;
            case 2:
                listarLogs();
                break;
            case 3:
                registrarUsuario();
                break;
            case 4:
                modificarUsuario();  // Nueva función que vamos a implementar
                break;
            case 5:
                eliminarUsuario();  // Nueva función que vamos a implementar
                break;
            case 0:
                printf("Cerrando sesión de Administrador...\n");
                return;
            default:
                printf("Opción no válida. Intente nuevamente.\n");
        }
    } while(opcion != 0);
}


