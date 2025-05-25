#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usuarios.h"
#include "logs.h"
#include "utils.h"
#include <sqlite3.h>
#include "database.h"  

int usuarioActualId = -1;
char tipoUsuarioActual[20] = "";

void registrarUsuario() {
    Usuario nuevoUsuario;

    printf("Ingrese el nombre del usuario: ");
    fgets(nuevoUsuario.nombre, 50, stdin);
    nuevoUsuario.nombre[strcspn(nuevoUsuario.nombre, "\n")] = 0;

    printf("Ingrese la contraseña: ");
    fgets(nuevoUsuario.password, 50, stdin);
    nuevoUsuario.password[strcspn(nuevoUsuario.password, "\n")] = 0;

    printf("Ingrese el tipo de usuario (Paciente/Medico/Admin): ");
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

    // Sentencia SQL con placeholders
    const char *sql = "INSERT INTO Usuario (nombre, tipo, password, email, telefono, direccion, fecha_registro) "
                      "VALUES (?, ?, ?, ?, ?, ?, datetime('now'));";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error al preparar la consulta: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Asignar valores a los parámetros
    sqlite3_bind_text(stmt, 1, nuevoUsuario.nombre, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, nuevoUsuario.tipo, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, nuevoUsuario.password, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, nuevoUsuario.email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, nuevoUsuario.telefono, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, nuevoUsuario.direccion, -1, SQLITE_STATIC);

    // Ejecutar
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        printf("Error al registrar usuario: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Usuario registrado exitosamente en la base de datos.\n");
    }

    sqlite3_finalize(stmt);
}

int autenticarUsuario() {
    char nombre[50];
    char password[50];

    printf("Nombre de usuario: ");
    fgets(nombre, sizeof(nombre), stdin);
    nombre[strcspn(nombre, "\n")] = 0;

    printf("Contraseña: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    const char *sql = "SELECT id, tipo FROM Usuario WHERE nombre = ? AND password = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error preparando consulta: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, nombre, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);

    int userId = -1;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        userId = sqlite3_column_int(stmt, 0);
        const unsigned char *tipo = sqlite3_column_text(stmt, 1);
        strcpy(tipoUsuarioActual, (const char*)tipo);

        printf("Autenticación exitosa.\n");
    } else {
        printf("Autenticación fallida.\n");
    }

    sqlite3_finalize(stmt);
    return userId;
}

void listarUsuarios() {
    const char *sql = "SELECT id, nombre, tipo, email, telefono, direccion, fecha_registro FROM Usuario;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error al preparar la consulta: %s\n", sqlite3_errmsg(db));
        return;
    }

    printf("======= LISTADO DE USUARIOS =======\n");

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char *nombre = sqlite3_column_text(stmt, 1);
        const unsigned char *tipo = sqlite3_column_text(stmt, 2);
        const unsigned char *email = sqlite3_column_text(stmt, 3);
        const unsigned char *telefono = sqlite3_column_text(stmt, 4);
        const unsigned char *direccion = sqlite3_column_text(stmt, 5);
        const unsigned char *fecha_registro = sqlite3_column_text(stmt, 6);

        printf("ID: %d\n", id);
        printf("Nombre: %s\n", nombre);
        printf("Tipo: %s\n", tipo);
        printf("Email: %s\n", email);
        printf("Telefono: %s\n", telefono);
        printf("Direccion: %s\n", direccion);
        printf("Fecha de Registro: %s\n", fecha_registro);
        printf("-------------------------------\n");
    }

    sqlite3_finalize(stmt);
}

void modificarUsuario() {
    int userId;
    listarUsuarios();  // Mostrar los usuarios para elegir

    printf("Ingrese el ID del usuario que desea modificar: ");
    scanf("%d", &userId);
    getchar();  // Limpiar el buffer

    char nombre[50], tipo[20], email[50], telefono[15], direccion[100], password[50];

    printf("Ingrese el nuevo nombre del usuario: ");
    fgets(nombre, sizeof(nombre), stdin);
    nombre[strcspn(nombre, "\n")] = 0;

    printf("Ingrese el nuevo tipo de usuario: ");
    fgets(tipo, sizeof(tipo), stdin);
    tipo[strcspn(tipo, "\n")] = 0;

    printf("Ingrese el nuevo email: ");
    fgets(email, sizeof(email), stdin);
    email[strcspn(email, "\n")] = 0;

    printf("Ingrese el nuevo teléfono: ");
    fgets(telefono, sizeof(telefono), stdin);
    telefono[strcspn(telefono, "\n")] = 0;

    printf("Ingrese la nueva dirección: ");
    fgets(direccion, sizeof(direccion), stdin);
    direccion[strcspn(direccion, "\n")] = 0;

    printf("Ingrese la nueva contraseña: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    const char *sql = "UPDATE Usuario SET nombre = ?, tipo = ?, email = ?, telefono = ?, direccion = ?, password = ? WHERE id = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error preparando consulta: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, nombre, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, tipo, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, telefono, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, direccion, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, password, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 7, userId);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Usuario con ID %d modificado exitosamente.\n", userId);
    } else {
        printf("Error al modificar usuario: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}


void eliminarUsuario() {
    int userId;
    listarUsuarios();  // Mostrar los usuarios actuales

    printf("Ingrese el ID del usuario que desea eliminar: ");
    scanf("%d", &userId);
    getchar();  // Limpiar el buffer

    const char *sql = "DELETE FROM Usuario WHERE id = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error preparando consulta DELETE: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, userId);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Usuario con ID %d eliminado exitosamente.\n", userId);
    } else {
        printf("Error al eliminar usuario: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

void listarMedicos() {
    const char *sql = "SELECT id, nombre FROM Usuario WHERE tipo = 'Medico';";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error al preparar la consulta: %s\n", sqlite3_errmsg(db));
        return;
    }

    printf("\n======= LISTA DE MÉDICOS =======\n");

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char *nombre = sqlite3_column_text(stmt, 1);
        printf("ID: %d, Nombre: %s\n", id, nombre);
    }

    printf("--------------------------------\n");
    sqlite3_finalize(stmt);
}
