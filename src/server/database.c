#include <stdio.h>
#include "database.h"

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
