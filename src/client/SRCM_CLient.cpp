// SRCM_Client.cpp - Cliente funcional con menú extendido para médicos
#include <iostream>
#include <winsock2.h>
#include <string>
#include <sstream>
#include <vector>
#include <time.h>
#include "server/horas.h"
#include <algorithm>

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
    string resultado;
    char buffer[1024];
    int len;

    do {
        len = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (len <= 0) break;
        buffer[len] = '\0';
        resultado += buffer;
    } while (len == sizeof(buffer) - 1);

    return resultado;
}

void login() {
    string user, pass;
    cout << "Usuario: "; getline(cin, user);
    cout << "Password: "; getline(cin, pass);
    enviar("LOGIN " + user + " " + pass);
    string r = recibir();

    if (r.find("Inicio de sesión correcto") != string::npos) {
        size_t pos = r.find("Rol:");
        if (pos != string::npos) {
            tipoUsuario = r.substr(pos + 4);
            tipoUsuario.erase(remove_if(tipoUsuario.begin(), tipoUsuario.end(), [](char c) {
                return !isprint(static_cast<unsigned char>(c)) || isspace(static_cast<unsigned char>(c));
            }), tipoUsuario.end());
        } else {
            tipoUsuario = "";
        }
        nombreUsuario = user;
        conectado = true;
        cout << r;
    } else {
        conectado = false;
        cout << "Usuario o contraseña incorrectos. Intente nuevamente.\n";
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
            enviar("LOGIN " + nombre + " " + pass);
            string r = recibir();
            if (r.find("Inicio de sesión correcto") != string::npos) {
                size_t pos = r.find("Rol:");
                if (pos != string::npos) {
                    tipoUsuario = r.substr(pos + 4);
                    tipoUsuario.erase(remove_if(tipoUsuario.begin(), tipoUsuario.end(), [](char c) {
                        return !isprint(static_cast<unsigned char>(c)) || isspace(static_cast<unsigned char>(c));
                    }), tipoUsuario.end());
                }
                nombreUsuario = nombre;
                conectado = true;
                cout << r;
            } else {
                conectado = false;
                cout << "No se pudo iniciar sesión automáticamente después del registro.\n";
            }
            break;
        }
    }
}

void reservarCitaPasoAPaso() {
    int mes, dia, indiceHora, medicoId;
    string motivo;

    cout << "Ingrese el mes para reservar (1-6 próximos meses): ";
    cin >> mes; cin.ignore();
    if (mes < 1 || mes > 6) {
        cout << "Mes inválido.\n";
        return;
    }

    time_t t = time(NULL);
    struct tm *fechaActual = localtime(&t);
    int mesActual = fechaActual->tm_mon + 1;
    int anioActual = fechaActual->tm_year + 1900;
    int mesReal = (mesActual + mes - 1) % 12;
    if (mesReal == 0) mesReal = 12;
    int anio = anioActual + ((mesActual + mes - 1) > 12 ? 1 : 0);

    stringstream cmd;
    cmd << "CALENDARIO " << mesReal << " " << anio;
    enviar(cmd.str());
    cout << recibir();

    cout << "Ingrese el día: ";
    cin >> dia; cin.ignore();
    if (dia < 1 || dia > 31) {
        cout << "Día inválido.\n";
        return;
    }

    cmd.str(""); cmd.clear();
    cmd << "HORAS " << dia << " " << mesReal << " " << anio;
    enviar(cmd.str());
    cout << recibir();

    cout << "Ingrese el índice de hora (0-23): ";
    cin >> indiceHora; cin.ignore();
    if (indiceHora < 0 || indiceHora >= 24) {
        cout << "Índice de hora inválido.\n";
        return;
    }
    
    enviar("LIST-MEDICOS");
    cout << recibir();

    cout << "Ingrese el ID del médico: ";
    cin >> medicoId; cin.ignore();

    cout << "Ingrese el motivo de la cita: ";
    getline(cin, motivo);

    cmd.str(""); cmd.clear();
    cmd << "RESERVAR " << mesReal << " " << dia << " " << anio << " "
        << indiceHora << " " << medicoId << " " << motivo;
    enviar(cmd.str());

    string respuesta = recibir();
    cout << respuesta;

    if (respuesta.find("No puedes") != string::npos ||
        respuesta.find("inválido") != string::npos ||
        respuesta.find("ya tiene una cita") != string::npos ||
        respuesta.find("Error") != string::npos) {
        cout << "Volviendo al menú...\n";
        return;
    }
}

void menuPaciente() {
    while (conectado) {
        cout << "\n--- MENU PACIENTE ---\n";
        cout << "1. Reservar cita\n";
        cout << "2. Consultar citas\n";
        cout << "3. Modificar cita\n";
        cout << "4. Cancelar cita\n";
        cout << "5. Ver historial\n";
        cout << "6. Cerrar sesión\n> ";

        int op;
        if (!(cin >> op)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Entrada inválida. Intenta de nuevo.\n";
            continue;
        }
        cin.ignore();

        switch (op) {
            case 1:
                reservarCitaPasoAPaso();
                break;
            case 2:
                enviar("LIST-CITAS");
                cout << recibir();
                break;
            case 3: {
                string id, nueva;
                cout << "ID cita: "; getline(cin, id);
                cout << "Nueva fecha: "; getline(cin, nueva);
                enviar("MODIFY " + id + " " + nueva);
                cout << recibir();
                break;
            }
            case 4: {
                string id;
                cout << "ID cita: "; getline(cin, id);
                enviar("CANCEL " + id);
                cout << recibir();
                break;
            }
            case 5:
                enviar("HISTORY");
                cout << recibir();
                break;
            case 6:
                enviar("LOGOUT");
                cout << recibir();
                tipoUsuario = "";
                conectado = false;
                return;
            default:
                cout << "Opción no válida.\n";
        }
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
