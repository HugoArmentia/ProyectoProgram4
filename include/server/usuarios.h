#ifndef USUARIOS_H
#define USUARIOS_H

typedef struct {
    char nombre[50];
    char tipo[20];
    char password[50];
    char email[50];
    char telefono[15];
    char direccion[100];
} Usuario;


extern int usuarioActualId; 
extern char tipoUsuarioActual[20];

void registrarUsuario();
int autenticarUsuario();
void listarUsuarios();
void modificarUsuario();
void eliminarUsuario();
void listarMedicos();

#endif
