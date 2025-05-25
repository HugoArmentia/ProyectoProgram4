// SRCM_Client.cpp - Cliente funcional con menú extendido para médicos
#include <iostream>
#include <winsock2.h>
#include <string>
#include <sstream>
#include <vector>

//#pragma comment(lib, "ws2_32.lib")

using namespace std;

SOCKET sock;
string tipoUsuario = "";
string nombreUsuario = "";
bool conectado = false;

bool conectar(const string &ip, int puerto) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return false;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip.c_str());
    server.sin_port = htons(puerto);
    return connect(sock, (sockaddr *)&server, sizeof(server)) >= 0;
}

void enviar(const string &msg) {
    string data = msg + "\n";
    send(sock, data.c_str(), data.size(), 0);
}

string recibir() {
    char buffer[2048];
    int len = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (len <= 0) {
        // Si len == 0, conexión cerrada; si len < 0, error
        return "";
    }
    buffer[len] = '\0';
    return string(buffer);
}

void login() {
    string user, pass;
    cout << "Usuario: "; getline(cin, user);
    cout << "Password: "; getline(cin, pass);
    enviar("LOGIN " + user + " " + pass);
    string r = recibir();
    if (r.empty()) {
        conectado = false;
        cout << "Conexión perdida con el servidor durante el login.\n";
        return;
    }
    cout << r;
    if (r.find("OK") == 0) {
        tipoUsuario = r.substr(3);
        tipoUsuario.erase(tipoUsuario.find_last_not_of(" \n\r") + 1);
        nombreUsuario = user; // <-- guardar nombre de usuario
        conectado = true;
    }
}

void registrarse() {
    while (true) {
        string nombre, tipo, pass;
        cout << "Nombre: "; getline(cin, nombre);
        cout << "Tipo (Paciente/Medico): "; getline(cin, tipo);
        cout << "Password: "; getline(cin, pass);

        enviar("REGISTER " + nombre + " " + tipo + " " + pass);
        string respuesta = recibir();
        if (respuesta.empty()) {
            conectado = false;
            cout << "Conexión perdida con el servidor durante el registro.\n";
            return;
        }
        cout << respuesta;

        if (respuesta.find("ERROR") != string::npos && 
            respuesta.find("ya existe") != string::npos) {
            cout << "Ese nombre ya está registrado. ¿Deseas intentar con otro? (s/n): ";
            string opcion; getline(cin, opcion);
            if (opcion != "s" && opcion != "S") break;
        } else {
            break;
        }
    }
}

void menuPaciente() {
    while (true) {
        cout << "\n--- MENU PACIENTE ---\n1. Reservar cita\n2. Consultar citas\n3. Modificar cita\n4. Cancelar cita\n5. Ver historial\n6. Cerrar sesión\n> ";
        int op; cin >> op; cin.ignore();
        if (op == 1) {
            string id, fecha;
            cout << "ID del medico: "; getline(cin, id);
            cout << "Fecha (YYYY-MM-DD HH:MM): "; getline(cin, fecha);
            enviar("BOOK " + id + " " + fecha);
        } else if (op == 2) {
            enviar("LIST-CITAS");
        } else if (op == 3) {
            string id, nueva;
            cout << "ID cita: "; getline(cin, id);
            cout << "Nueva fecha: "; getline(cin, nueva);
            enviar("MODIFY " + id + " " + nueva);
        } else if (op == 4) {
            string id;
            cout << "ID cita: "; getline(cin, id);
            enviar("CANCEL " + id);
        } else if (op == 5) {
            enviar("HISTORY");
        } else if (op == 6) {
            enviar("LOGOUT");
            tipoUsuario = "";
            conectado = false;
            break;
        }
        cout << recibir();
        if (!conectado) break;
    }
}

void menuMedico() {
    while (true) {
        cout << "\n--- MENU MEDICO ---\n1. Ver citas asignadas\n2. Ver historial\n3. Completar cita\n4. Cerrar sesión\n> ";
        int op; cin >> op; cin.ignore();
        if (op == 1) {
            enviar("LIST-CITAS");
        } else if (op == 2) {
            enviar("HISTORY");
        } else if (op == 3) {
            string id;
            cout << "ID de la cita a completar: ";
            getline(cin, id);
            enviar("COMPLETAR " + id);
        } else if (op == 4) {
            enviar("LOGOUT");
            tipoUsuario = "";
            conectado = false;
            break;
        }
        cout << recibir();
        if (!conectado) break; 
    }
}

void menuAdmin() {
    if (tipoUsuario != "Admin" || nombreUsuario != "admin") {

        cout << "Acceso denegado. Solo el administrador puede usar este menú.\n";
        return;
    }

    while (true) {
        cout << "\n--- MENU ADMIN ---\n";
        cout << "1. Listar usuarios\n";
        cout << "2. Ver logs\n";
        cout << "3. Crear nuevo usuario\n";
        cout << "4. Modificar usuario\n";
        cout << "5. Eliminar usuario\n";
        cout << "6. Apagar servidor\n";
        cout << "0. Cerrar sesión\n> ";

        int op; cin >> op; cin.ignore();

        if (op == 1) {
            enviar("LIST-USERS");
        } else if (op == 2) {
            enviar("VIEW-LOGS");
        } else if (op == 3) {
            string nombre, tipo, pass;
            cout << "Nombre: "; getline(cin, nombre);
            cout << "Tipo (Paciente/Medico): "; getline(cin, tipo);
            cout << "Password: "; getline(cin, pass);
            enviar("REGISTER " + nombre + " " + tipo + " " + pass);
        } else if (op == 4) {
            string id, nuevoNombre;
            cout << "ID del usuario a modificar: "; getline(cin, id);
            cout << "Nuevo nombre: "; getline(cin, nuevoNombre);
            enviar("MODIFY-USER " + id + " " + nuevoNombre);
        } else if (op == 5) {
            string id;
            cout << "ID del usuario a eliminar: "; getline(cin, id);
            enviar("DELETE-USER " + id);
        } else if (op == 6) {
            enviar("SHUTDOWN");
            conectado = false;
            break;
        } else if (op == 0) {
            enviar("LOGOUT");
            tipoUsuario = "";
            conectado = false;
            break;
        } else {
            cout << "Opción no válida. Intente nuevamente.\n";
            continue;
        }

        cout << recibir();
        if (!conectado) break; 
    }
}


int main() {
    if (!conectar("127.0.0.1", 5000)) {
        cerr << "No se pudo conectar al servidor.\n";
        return 1;
    }

    while (true) {
        if (!conectado) {
            cout << "\n--- MENU INICIAL ---\n1. Iniciar sesión\n2. Registrarse\n3. Salir\n> ";
            int op; cin >> op; cin.ignore();
            if (op == 1) login();
            else if (op == 2) registrarse();
            else break;
        } else {
            if (tipoUsuario == "Paciente") {
                menuPaciente();
            }
            else if (tipoUsuario == "Medico") {
                menuMedico();
            }
            else if (tipoUsuario == "Admin") {
                menuAdmin();
            }
            else {
                cout << "Tipo de usuario inválido.\n";
                conectado = false;
            }
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
