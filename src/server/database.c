#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "database.h"

// Declaración de la variable global db
sqlite3 *db;

int inicializarBaseDeDatos() {
    int rc = sqlite3_open("data/1.db", &db);  // Ruta a tu archivo .db creado en DB Browser for SQLite
    if (rc != SQLITE_OK) {
        printf("No se pudo abrir la base de datos: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    printf("Base de datos 1.db abierta correctamente.\n");
    return 1;
}

void cerrarBaseDeDatos() {
    if (db) {
        sqlite3_close(db);
        printf("Base de datos cerrada correctamente.\n");
    }
}
