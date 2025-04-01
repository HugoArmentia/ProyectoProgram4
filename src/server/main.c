#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h"
#include "logs.h"
#include "usuarios.h"
#include "sqlite3.h"
#include <sqlite3.h>
#include "database.h"

// Variables globales del usuario autenticado
int usuarioActualId = -1;
char tipoUsuarioActual[20] = "";

int main() {
    // Inicializar archivos necesarios
    inicializarArchivos();
    cargarUsuarios();
    cargarLogs();

    int opcion;

    do {
        printf("\n======= MENU PRINCIPAL =======\n");
        printf("1. Iniciar sesion\n");
        printf("2. Registrar nuevo usuario (Paciente/Medico/Admin)\n");
        printf("0. Salir\n");
        printf("Seleccione una opcion: ");
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



    if (!inicializarBaseDeDatos()) {
        printf("Error al inicializar la base de datos.\n");
        return 1;
    }

    sqlite3_stmt *stmt;
    const char *sql = "SELECT name FROM sqlite_master WHERE type='table';";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        printf("Tablas en la base de datos 1.db:\n");
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            printf("- %s\n", sqlite3_column_text(stmt, 0));
        }
        sqlite3_finalize(stmt);
    } else {
        printf("Error al obtener las tablas: %s\n", sqlite3_errmsg(db));
    }

    cerrarBaseDeDatos();
    return 0;

    
}
