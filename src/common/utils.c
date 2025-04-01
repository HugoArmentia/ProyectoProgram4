#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "utils.h"

// Obtener la fecha y hora actual en formato: "YYYY-MM-DD HH:MM:SS"
void obtenerFechaHoraActual(char *fechaHora) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(fechaHora, "%04d-%02d-%02d %02d:%02d:%02d",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec);
}

// Validar si una cadena es un número
int esNumero(const char *cadena) {
    for (int i = 0; cadena[i] != '\0'; i++) {
        if (!isdigit(cadena[i])) {
            return 0;
        }
    }
    return 1;
}

// Limpiar el buffer de entrada
void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Convertir una cadena a minúsculas
void convertirMinusculas(char *cadena) {
    for (int i = 0; cadena[i]; i++) {
        cadena[i] = tolower(cadena[i]);
    }
}

// Convertir una cadena a mayúsculas
void convertirMayusculas(char *cadena) {
    for (int i = 0; cadena[i]; i++) {
        cadena[i] = toupper(cadena[i]);
    }
}

// Recortar espacios en blanco al inicio y al final
void recortarEspacios(char *cadena) {
    char *inicio = cadena;
    char *fin = cadena + strlen(cadena) - 1;

    while (isspace((unsigned char)*inicio)) inicio++;
    while (fin > inicio && isspace((unsigned char)*fin)) fin--;
    *(fin + 1) = '\0';

    if (inicio != cadena) {
        memmove(cadena, inicio, fin - inicio + 2);
    }
}
