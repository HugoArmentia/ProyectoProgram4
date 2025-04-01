#ifndef USUARIOS_H
#define USUARIOS_H

#define MAX_USUARIOS 100

typedef struct {
    int id;
    char nombre[50];
    char tipo[20];
    char password[50];
    char email[50];
    char telefono[15];
    char direccion[100];
    char fecha_registro[20];
} Usuario;

extern Usuario usuarios[MAX_USUARIOS];
extern int totalUsuarios;

// Funciones relacionadas con usuarios
void cargarUsuarios();               // Función sin parámetros (sin conflictos)
void guardarUsuarios();
void registrarUsuario();
int autenticarUsuario();
void listarUsuarios();

#endif
