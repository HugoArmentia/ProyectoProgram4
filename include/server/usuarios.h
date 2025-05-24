#ifndef USUARIOS_H
#define USUARIOS_H

extern int usuarioActualId; 
extern char tipoUsuarioActual[20];

void cargarUsuarios();              
void guardarUsuarios();
void registrarUsuario();
int autenticarUsuario();
void listarUsuarios();
void modificarUsuario();
void eliminarUsuario();
void listarMedicos();

#endif
