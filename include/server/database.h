#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

extern sqlite3 *db;

int inicializarBaseDeDatos();
void cerrarBaseDeDatos();

int ejecutarConsultaSQL(const char *sql);
void crearTablas();


#endif
