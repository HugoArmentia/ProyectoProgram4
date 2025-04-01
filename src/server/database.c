#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "database.h"

// Declaración de la variable global db
sqlite3 *db;

// Función para inicializar (abrir) la base de datos
int inicializarBaseDeDatos() {
    int rc = sqlite3_open("data/citas_medicas.db", &db);  // Ruta a tu archivo .db creado en DB Browser for SQLite
    if (rc != SQLITE_OK) {
        printf("No se pudo abrir la base de datos: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    printf("Base de datos 1.db abierta correctamente.\n");
    return 1;
}

// Función para cerrar la base de datos
void cerrarBaseDeDatos() {
    if (db) {
        sqlite3_close(db);  // Cerrar la base de datos
        printf("Base de datos cerrada correctamente.\n");
        db = NULL;  // Asegurarse de que db no quede apuntando a una dirección inválida
    } else {
        printf("La base de datos ya está cerrada o no fue inicializada correctamente.\n");
    }
}

// Función para ejecutar una consulta SQL (ejemplo genérico)
int ejecutarConsultaSQL(const char *sql) {
    char *errMsg = NULL;
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error al ejecutar la consulta: %s\n", errMsg);
        if (errMsg) {
            sqlite3_free(errMsg);  // Liberar mensaje de error si existe
        }
        return 0;
    }
    printf("Consulta ejecutada correctamente\n");
    return 1;
}
