#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

// Longitud máxima de la cadena para la fecha y hora
#define MAX_FECHA_HORA 20  

// Funciones para manipulación de cadenas
void obtenerFechaHoraActual(char *fechaHora);         // Obtener la fecha y hora actual en formato: "YYYY-MM-DD HH:MM:SS"
int esNumero(const char *cadena);                     // Validar si una cadena es un número
void limpiarBuffer();                                 // Limpiar el buffer de entrada (stdin)
void convertirMinusculas(char *cadena);              // Convertir una cadena a minúsculas
void convertirMayusculas(char *cadena);              // Convertir una cadena a mayúsculas
void recortarEspacios(char *cadena);                 // Recortar espacios en blanco al inicio y al final                 

#endif
