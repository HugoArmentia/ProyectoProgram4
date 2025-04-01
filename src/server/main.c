#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h"
#include "database.h"
#include "logs.h"
#include "usuarios.h"

Usuario usuarios[MAX_USUARIOS];
int totalUsuarios = 0;
LogActividad logs[MAX_LOGS];
int totalLogs = 0;

int usuarioActualId = -1;
char tipoUsuarioActual[20] = "";

int main() {

    inicializarArchivos();
    cargarUsuarios();
    cargarLogs();

    int opcion;

    do {
        printf("\n======= MENU PRINCIPAL =======\n");
        printf("1. Iniciar sesión\n");
        printf("2. Registrar nuevo usuario\n");
        printf("0. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        getchar();

        switch(opcion) {
            case 1:
                usuarioActualId = autenticarUsuario();
                
                if (usuarioActualId != -1) {
                    strcpy(tipoUsuarioActual, usuarios[usuarioActualId - 1].tipo);
                    
                    if (strcmp(tipoUsuarioActual, "Paciente") == 0) {
                        mostrarMenuPaciente();
                    } else if (strcmp(tipoUsuarioActual, "Medico") == 0) {
                        mostrarMenuMedico();
                    } else if (strcmp(tipoUsuarioActual, "Admin") == 0) {
                        mostrarMenuAdmin();
                    }
                }
                break;

            case 2:
                registrarUsuario();
                break;

            case 0:
                printf("Saliendo del programa...\n");
                break;

            default:
                printf("Opción no válida. Intente nuevamente.\n");
        }
    } while(opcion != 0);

    guardarUsuarios();
    guardarLogs();

    printf("Gracias por usar el Sistema de Reservas de Citas Médicas (SRCM).\n");
    return 0;
}
