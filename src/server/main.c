#include <stdio.h>
#include "menu.h"
#include "database.h"
#include "usuarios.h"
#include "citas.h"
#include "historial.h"
#include "logs.h"

int main() {
    inicializarArchivos();  // Crear archivos si no existen
    cargarUsuarios();
    cargarCitas();
    cargarHistorial();
    cargarLogs();

    mostrarMenuPrincipal();

    guardarUsuarios();
    guardarCitas();
    guardarHistorial();
    guardarLogs();

    return 0;
}
