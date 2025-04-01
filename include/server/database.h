#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

// Declarar la variable db como extern para que sea visible en otros archivos .c
extern sqlite3 *db;

// Declaraciones de funciones para inicializar y cerrar la base de datos
int inicializarBaseDeDatos();  // Abre la base de datos
void cerrarBaseDeDatos();      // Cierra la base de datos

// Función para ejecutar una consulta SQL genérica
int ejecutarConsultaSQL(const char *sql);

#endif
