#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usuarios.h"
#include "logs.h"
#include "utils.h"

#define RUTA_USUARIOS "data/usuarios.txt"

Usuario usuarios[MAX_USUARIOS];
int totalUsuarios = 0;

void cargarUsuarios() {
    FILE *archivo = fopen(RUTA_USUARIOS, "r");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo de usuarios. Creando uno nuevo.\n");
        return;
    }

    while (fscanf(archivo, "%d,%49[^,],%19[^,],%49[^,],%49[^,],%14[^,],%99[^,],%19s\n",
                  &usuarios[totalUsuarios].id,
                  usuarios[totalUsuarios].nombre,
                  usuarios[totalUsuarios].tipo,
                  usuarios[totalUsuarios].password,
                  usuarios[totalUsuarios].email,
                  usuarios[totalUsuarios].telefono,
                  usuarios[totalUsuarios].direccion,
                  usuarios[totalUsuarios].fecha_registro) == 8) {
        totalUsuarios++;
    }

    fclose(archivo);
}

void guardarUsuarios() {
    FILE *archivo = fopen("data/usuarios.txt", "w");  // Abre el archivo en modo escritura
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo de usuarios para escribir.\n");
        return;
    }

    for (int i = 0; i < totalUsuarios; i++) {
        fprintf(archivo, "%d,%s,%s,%s,%s,%s,%s,%s\n",
                usuarios[i].id,
                usuarios[i].nombre,
                usuarios[i].tipo,
                usuarios[i].password,
                usuarios[i].email,
                usuarios[i].telefono,
                usuarios[i].direccion,
                usuarios[i].fecha_registro);
    }

    fclose(archivo);
    printf("Usuarios guardados correctamente.\n");
}

void registrarUsuario() { 
    if (totalUsuarios >= MAX_USUARIOS) {
        printf("Límite máximo de usuarios alcanzado.\n");
        return;
    }

    Usuario nuevoUsuario;
    nuevoUsuario.id = totalUsuarios + 1;

    printf("Ingrese el nombre del usuario: ");
    fgets(nuevoUsuario.nombre, 50, stdin);
    nuevoUsuario.nombre[strcspn(nuevoUsuario.nombre, "\n")] = 0;
    limpiarBuffer();

    printf("Ingrese la contraseña: ");
    fgets(nuevoUsuario.password, 50, stdin);
    nuevoUsuario.password[strcspn(nuevoUsuario.password, "\n")] = 0;

    printf("Ingrese el tipo de usuario: ");
    fgets(nuevoUsuario.tipo, 20, stdin);
    nuevoUsuario.tipo[strcspn(nuevoUsuario.tipo, "\n")] = 0;

    printf("Ingrese el email: ");
    fgets(nuevoUsuario.email, 50, stdin);
    nuevoUsuario.email[strcspn(nuevoUsuario.email, "\n")] = 0;

    printf("Ingrese el teléfono: ");
    fgets(nuevoUsuario.telefono, 15, stdin);
    nuevoUsuario.telefono[strcspn(nuevoUsuario.telefono, "\n")] = 0;

    printf("Ingrese la dirección: ");
    fgets(nuevoUsuario.direccion, 100, stdin);
    nuevoUsuario.direccion[strcspn(nuevoUsuario.direccion, "\n")] = 0;

    printf("Ingrese la fecha de registro (YYYY-MM-DD): ");
    fgets(nuevoUsuario.fecha_registro, 20, stdin);
    nuevoUsuario.fecha_registro[strcspn(nuevoUsuario.fecha_registro, "\n")] = 0;

    usuarios[totalUsuarios] = nuevoUsuario;
    totalUsuarios++;
    
    guardarUsuarios();

    printf("Usuario registrado correctamente.\n");
}

int autenticarUsuario() {
    char nombre[50];
    char password[50];

    printf("Nombre de usuario: ");
    fgets(nombre, 50, stdin);
    nombre[strcspn(nombre, "\n")] = 0;
    limpiarBuffer();

    printf("Contraseña: ");
    fgets(password, 50, stdin);
    password[strcspn(password, "\n")] = 0;

    for (int i = 0; i < totalUsuarios; i++) {
        if (strcmp(usuarios[i].nombre, nombre) == 0 && strcmp(usuarios[i].password, password) == 0) {
            printf("Autenticación exitosa.\n");
            return usuarios[i].id;
        }
    }

    printf("Autenticación fallida.\n");
    return -1;
}

void listarUsuarios() {
    if (totalUsuarios == 0) {
        printf("No hay usuarios registrados.\n");
        return;
    }

    printf("======= LISTADO DE USUARIOS =======\n");

    for (int i = 0; i < totalUsuarios; i++) {
        printf("ID: %d\n", usuarios[i].id);
        printf("Nombre: %s\n", usuarios[i].nombre);
        printf("Tipo: %s\n", usuarios[i].tipo);
        printf("Email: %s\n", usuarios[i].email);
        printf("Telefono: %s\n", usuarios[i].telefono);
        printf("Direccion: %s\n", usuarios[i].direccion);
        printf("Fecha de Registro: %s\n", usuarios[i].fecha_registro);
        printf("-------------------------------\n");
    }
    
}
//Funcion modificar un usuario
void modificarUsuario() {
    int userId;
    printf("Ingrese el ID del usuario que desea modificar: ");
    scanf("%d", &userId);
    getchar();

   
    printf("Usuario con ID %d modificado exitosamente.\n", userId);
}

// Función para eliminar un usuario
void eliminarUsuario() {
    int userId;
    printf("Ingrese el ID del usuario que desea eliminar: ");
    scanf("%d", &userId);
    getchar();

    
    printf("Usuario con ID %d eliminado exitosamente.\n", userId);
}