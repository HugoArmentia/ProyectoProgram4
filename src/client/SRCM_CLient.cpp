// SRCM_Client.cpp - Cliente funcional con menú extendido para médicos
#include <iostream>
#include <winsock2.h>
#include <string>
#include <sstream>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

SOCKET sock;
string tipoUsuario = "";
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
    if (len <= 0) return "";
    buffer[len] = '\0';
    return string(buffer);
}

void login() {
    string user, pass;
    cout << "Usuario: "; getline(cin, user);
    cout << "Password: "; getline(cin, pass);
    enviar("LOGIN " + user + " " + pass);
    string r = recibir();
    cout << r;
    if (r.find("OK") == 0) {
        tipoUsuario = r.substr(3);
        tipoUsuario.erase(tipoUsuario.find_last_not_of(" \n\r") + 1);
        conectado = true;
    }
}

void registrarse() {
    string nombre, tipo, pass;
    cout << "Nombre: "; getline(cin, nombre);
    cout << "Tipo (Paciente/Medico): "; getline(cin, tipo);
    cout << "Password: "; getline(cin, pass);
    enviar("REGISTER " + nombre + " " + tipo + " " + pass);
    cout << recibir();
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
            if (tipoUsuario == "Paciente") menuPaciente();
            else if (tipoUsuario == "Medico") menuMedico();
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
