#include <stdio.h>
#include "menu.h"
#include "usuarios.h"
#include "citas.h"
#include "historial.h"
#include "logs.h"
#include "database.h"

void mostrarMenuPrincipal() {
    int opcion;

    do {
        printf("\n==== MENÚ PRINCIPAL ====\n");
        printf("1. Registrar Usuario\n");
        printf("2. Iniciar Sesión\n");
        printf("3. Crear Cita\n");
        printf("4. Listar Citas\n");
        printf("5. Cancelar Cita\n");
        printf("6. Mostrar Historial\n");
        printf("7. Mostrar Logs\n");
        printf("8. Salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                registrarUsuario();
                break;
            case 2:
                autenticarUsuario();
                break;
            case 3:
                crearCita();
                break;
            case 4:
                listarCitas();
                break;
            case 5:
                cancelarCita();
                break;
            case 6:
                listarHistorial();
                break;
            case 7:
                listarLogs();
                break;
            case 8:
                printf("Saliendo del sistema...\n");
                break;
            default:
                printf("Opción no válida, intente nuevamente.\n");
        }
    } while (opcion != 8);
}
