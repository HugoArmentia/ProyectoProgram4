#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "database.h"
#include "config.h"
extern Config configuracion;

sqlite3 *db;

void crearTablas() {
    const char *sql =
        "BEGIN TRANSACTION;"

        "CREATE TABLE IF NOT EXISTS Usuario ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "nombre TEXT NOT NULL,"
        "tipo TEXT NOT NULL CHECK(tipo IN ('Paciente', 'Medico', 'Admin')),"
        "password TEXT NOT NULL,"
        "email TEXT,"
        "telefono TEXT,"
        "direccion TEXT,"
        "fecha_registro TEXT DEFAULT (datetime('now'))"
        ");"

        "CREATE TABLE IF NOT EXISTS citas ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "paciente_id INTEGER NOT NULL,"
        "medico_id INTEGER NOT NULL,"
        "fecha TEXT NOT NULL,"
        "estado TEXT CHECK(estado IN ('Programada', 'Completada', 'Cancelada')),"
        "motivo TEXT,"
        "fecha_modificacion TEXT DEFAULT (datetime('now')),"
        "dia INTEGER,"
        "mes INTEGER,"
        "anio INTEGER,"
        "FOREIGN KEY(medico_id) REFERENCES Usuario(id) ON DELETE CASCADE,"
        "FOREIGN KEY(paciente_id) REFERENCES Usuario(id) ON DELETE CASCADE"
        ");"


        "CREATE TABLE IF NOT EXISTS historial_citas ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "paciente_id INTEGER NOT NULL,"
        "medico_id INTEGER NOT NULL,"
        "fecha TEXT NOT NULL,"
        "estado TEXT CHECK(estado IN ('Programada', 'Completada', 'Cancelada')),"
        "motivo TEXT,"
        "fecha_modificacion TEXT DEFAULT (datetime('now')),"
        "FOREIGN KEY(medico_id) REFERENCES Usuario(id) ON DELETE CASCADE,"
        "FOREIGN KEY(paciente_id) REFERENCES Usuario(id) ON DELETE CASCADE"
        ");"

        "CREATE TABLE IF NOT EXISTS logs ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "tipo_evento TEXT NOT NULL,"
        "descripcion TEXT,"
        "usuario_id INTEGER,"
        "fecha_evento TEXT DEFAULT (datetime('now')),"
        "FOREIGN KEY(usuario_id) REFERENCES Usuario(id)"
        ");"

        "INSERT OR IGNORE INTO Usuario(nombre, tipo, password) VALUES ('admin', 'Admin', 'admin123');"

        "COMMIT;";

    char *errMsg = NULL;
    if (sqlite3_exec(db, sql, 0, 0, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "Error al crear las tablas: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("Tablas creadas correctamente (si no existían).\n");
    }
}

int inicializarBaseDeDatos() {
    int rc = sqlite3_open(configuracion.nombre_base_datos, &db);
    if (rc != SQLITE_OK) {
        printf("No se pudo abrir la base de datos: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    printf("Base de datos '%s' abierta correctamente.\n", configuracion.nombre_base_datos);
    return 1;
}

void cerrarBaseDeDatos() {
    if (db) {
        sqlite3_close(db);
        printf("Base de datos cerrada correctamente.\n");
        db = NULL;
    } else {
        printf("La base de datos ya está cerrada o no fue inicializada correctamente.\n");
    }
}

int ejecutarConsultaSQL(const char *sql) {
    char *errMsg = NULL;
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error al ejecutar la consulta: %s\n", errMsg);
        if (errMsg) {
            sqlite3_free(errMsg);
        }
        return 0;
    }
    printf("Consulta ejecutada correctamente\n");
    return 1;
}
