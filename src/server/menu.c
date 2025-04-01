#include <stdio.h>
#include <string.h>
#include "menu.h"
#include "usuarios.h"
#include "citas.h"
#include "historial.h"
#include "logs.h"

<<<<<<< HEAD
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
=======
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
        getchar();  

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
void mostrarMenuPaciente(int pacienteId) {  // Recibe el ID del paciente autenticado
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
        getchar();  // Limpiar el buffer

        switch(opcion) {
            case 1:
                crearCita(pacienteId);  // Función a implementar para reservar citas
                break;
            case 2:
                listarCitasPaciente(pacienteId);  // Mostrar citas del paciente autenticado
                break;
            case 3:
                modificarCita(pacienteId);  // Modificar una cita del paciente
                break;
            case 4:
                cancelarCita(pacienteId);  // Cancelar una cita del paciente
                break;
            case 5:
                listarHistorialPaciente(pacienteId);  // Mostrar historial de citas del paciente
>>>>>>> f2a9c839496782ec04ea612938c67c3fb6d2e685
                break;
            case 0:
                printf("Cerrando sesión de Paciente...\n");
                return;
            default:
                printf("Opción no válida. Intente nuevamente.\n");
        }
<<<<<<< HEAD
<<<<<<< HEAD
    } while(opcion != 0);
}

// Menú exclusivo para Médicos
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
                listarCitas();  // Mostrar citas asignadas al médico
                break;
            case 2:
                listarHistorial();  // Mostrar historial de citas relacionadas con el médico
                break;
            case 0:
                printf("Cerrando sesión de Medico...\n");
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
                listarUsuarios();  // Función que muestra todos los usuarios registrados
                break;
            case 2:
                listarLogs();  // Mostrar logs del sistema
                break;
            case 3:
                registrarUsuario();  // Permitir crear un nuevo usuario desde el menú Admin
                break;
            case 0:
                printf("Cerrando sesión de Administrador...\n");
                return;
            default:
                printf("Opción no válida. Intente nuevamente.\n");
        }
    } while(opcion != 0);
=======
    } while (opcion != 8);

>>>>>>> 8c2245aa7d554e2e6c659522b72638a9235d6d1e
=======
    } while(opcion != 0);
>>>>>>> f2a9c839496782ec04ea612938c67c3fb6d2e685
}

void mostrarMenuAdmin() {  // No necesita ID porque es un acceso total
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
                listarUsuarios();  // Mostrar todos los usuarios registrados
                break;
            case 2:
                listarLogs();  // Mostrar logs del sistema
                break;
            case 3:
                registrarUsuario();  // Permitir crear un nuevo usuario
                break;
            case 4:
                modificarUsuario();  // Función a implementar para modificar usuarios
                break;
            case 5:
                eliminarUsuario();  // Función a implementar para eliminar usuarios
                break;
            case 0:
                printf("Cerrando sesión de Administrador...\n");
                return;
            default:
                printf("Opción no válida. Intente nuevamente.\n");
        }
    } while(opcion != 0);
}


