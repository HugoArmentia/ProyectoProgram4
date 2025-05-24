#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h"
#include "logs.h"
#include "usuarios.h"
#include <sqlite3.h>
#include "database.h"

int usuarioActualId = -1;
char tipoUsuarioActual[20] = "";

int main() {
    if (!inicializarBaseDeDatos()) {
        printf("Error al inicializar la base de datos.\n");
        return 1;
    }

    // Mostrar tablas de depuración (opcional)
    sqlite3_stmt *stmt;
    const char *sql = "SELECT name FROM sqlite_master WHERE type='table';";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        printf("Tablas en la base de datos:\n");
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            printf("- %s\n", sqlite3_column_text(stmt, 0));
        }
        sqlite3_finalize(stmt);
    } else {
        printf("Error al obtener las tablas: %s\n", sqlite3_errmsg(db));
    }

    registrarLog("Sistema", "Sistema SRCM iniciado", -1);

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
                    registrarLog("Login", "Inicio de sesión exitoso", usuarioActualId);

                    if (strcmp(tipoUsuarioActual, "Paciente") == 0) {
                        mostrarMenuPaciente();
                    } else if (strcmp(tipoUsuarioActual, "Medico") == 0) {
                        mostrarMenuMedico();
                    } else if (strcmp(tipoUsuarioActual, "Admin") == 0) {
                        mostrarMenuAdmin();
                    }
                } else {
                    registrarLog("Login fallido", "Intento de autenticación fallido", -1);
                }
                break;

            case 2:
                registrarUsuario();
                registrarLog("Registro", "Nuevo usuario registrado desde menú principal", -1);
                break;

            case 0:
                printf("Saliendo del programa...\n");
                registrarLog("Logout", "Sistema cerrado correctamente", usuarioActualId);
                break;

            default:
                printf("Opción no válida. Intente nuevamente.\n");
                registrarLog("Error", "Selección inválida en menú principal", -1);
        }
    } while(opcion != 0);

    cerrarBaseDeDatos();

    printf("Gracias por usar el Sistema de Reservas de Citas Médicas (SRCM).\n");
    return 0;
}
