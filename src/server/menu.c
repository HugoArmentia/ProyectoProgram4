#include <stdio.h>
#include "menu.h"
#include "usuarios.h"
#include "citas.h"
#include "historial.h"
#include "logs.h"
#include "database.h"

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

