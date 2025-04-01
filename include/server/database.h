#ifndef DATABASE_H
#define DATABASE_H

#include "usuarios.h"
#include "citas.h"
#include "historial.h"
#include "logs.h"

// Funciones para manejo de archivos y base de datos
void inicializarArchivos();         // Inicializa los archivos de datos si no existen
void cargarUsuarios();               // Carga todos los usuarios desde el archivo
void guardarUsuarios();              // Guarda todos los usuarios en el archivo

void cargarCitas();                  // Carga todas las citas desde el archivo
void guardarCitas();                 // Guarda todas las citas en el archivo

void cargarHistorial();              // Carga el historial de citas desde el archivo
void guardarHistorial();             // Guarda el historial de citas en el archivo

void cargarLogs();                   // Carga los logs desde el archivo
void guardarLogs();                  // Guarda los logs en el archivo

#endif
