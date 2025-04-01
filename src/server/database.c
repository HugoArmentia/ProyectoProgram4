#include <stdio.h>
#include "database.h"

// Inicializa archivos si no existen (sin conflictos)
void inicializarArchivos() {
    FILE *archivo;

    archivo = fopen("data/usuarios.txt", "a");
    if (archivo) fclose(archivo);

    archivo = fopen("data/citas.txt", "a");
    if (archivo) fclose(archivo);

    archivo = fopen("data/historial_citas.txt", "a");
    if (archivo) fclose(archivo);

    archivo = fopen("data/logs_actividades.txt", "a");
    if (archivo) fclose(archivo);
}
