#include <iostream>
#include <winsock2.h>
#include <string>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

SOCKET clientSocket;

bool conectarAlServidor(const string& ip, int puerto) {
    WSADATA wsa;
    cout << "Inicializando Winsock..." << endl;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cerr << "Fallo en WSAStartup. Error: " << WSAGetLastError() << endl;
        return false;
    }

    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        cerr << "No se pudo crear el socket. Error: " << WSAGetLastError() << endl;
        return false;
    }

    sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(ip.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(puerto);

    if (connect(clientSocket, (struct sockaddr*)&server, sizeof(server)) < 0) {
        cerr << "Conexion fallida." << endl;
        return false;
    }

    cout << "Conectado al servidor." << endl;
    return true;
}

void enviarComando(const string& comando) {
    send(clientSocket, comando.c_str(), comando.length(), 0);
}

string recibirRespuesta() {
    char buffer[2048];
    memset(buffer, 0, sizeof(buffer));
    int bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) return "[Error de comunicacion]";
    return string(buffer);
}

void mostrarMenu() {
    cout << "\n---- MENU SRCM ----\n";
    cout << "1. Iniciar sesion\n";
    cout << "2. Registrarse\n";
    cout << "3. Reservar cita\n";
    cout << "4. Consultar citas\n";
    cout << "5. Modificar cita\n";
    cout << "6. Cancelar cita\n";
    cout << "7. Ver historial\n";
    cout << "8. Cerrar sesion\n";
    cout << "9. Salir\n";
    cout << "Seleccione una opcion: ";
}

int main() {
    string ip = "127.0.0.1";
    int puerto = 5000;

    if (!conectarAlServidor(ip, puerto)) return 1;

    string usuarioLogeado = "";
    bool conectado = true;

    while (conectado) {
        mostrarMenu();
        int opcion;
        cin >> opcion;
        cin.ignore();

        string comando, respuesta;

        switch (opcion) {
            case 1: {
                string usuario, password;
                cout << "Usuario: ";
                getline(cin, usuario);
                cout << "Password: ";
                getline(cin, password);
                comando = "LOGIN " + usuario + " " + password;
                enviarComando(comando);
                respuesta = recibirRespuesta();
                cout << respuesta << endl;
                if (respuesta.find("OK") != string::npos) usuarioLogeado = usuario;
                break;
            }
            case 2: {
                string nombre, tipo, password;
                cout << "Nombre: ";
                getline(cin, nombre);
                cout << "Tipo (Paciente/Medico): ";
                getline(cin, tipo);
                cout << "Password: ";
                getline(cin, password);
                comando = "REGISTER " + nombre + " " + tipo + " " + password;
                enviarComando(comando);
                cout << recibirRespuesta() << endl;
                break;
            }
            case 3: {
                if (usuarioLogeado.empty()) { cout << "Debes iniciar sesion.\n"; break; }
                string idMedico, fecha;
                cout << "ID del medico: ";
                getline(cin, idMedico);
                cout << "Fecha (YYYY-MM-DD HH:MM): ";
                getline(cin, fecha);
                comando = "BOOK " + idMedico + " " + fecha;
                enviarComando(comando);
                cout << recibirRespuesta() << endl;
                break;
            }
            case 4: {
                if (usuarioLogeado.empty()) { cout << "Debes iniciar sesion.\n"; break; }
                comando = "LIST-CITAS";
                enviarComando(comando);
                cout << recibirRespuesta() << endl;
                break;
            }
            case 5: {
                if (usuarioLogeado.empty()) { cout << "Debes iniciar sesion.\n"; break; }
                string idCita, nuevaFecha;
                cout << "ID de la cita: ";
                getline(cin, idCita);
                cout << "Nueva fecha (YYYY-MM-DD HH:MM): ";
                getline(cin, nuevaFecha);
                comando = "MODIFY " + idCita + " " + nuevaFecha;
                enviarComando(comando);
                cout << recibirRespuesta() << endl;
                break;
            }
            case 6: {
                if (usuarioLogeado.empty()) { cout << "Debes iniciar sesion.\n"; break; }
                string idCita;
                cout << "ID de la cita a cancelar: ";
                getline(cin, idCita);
                comando = "CANCEL " + idCita;
                enviarComando(comando);
                cout << recibirRespuesta() << endl;
                break;
            }
            case 7: {
                if (usuarioLogeado.empty()) { cout << "Debes iniciar sesion.\n"; break; }
                comando = "HISTORY";
                enviarComando(comando);
                cout << recibirRespuesta() << endl;
                break;
            }
            case 8: {
                if (usuarioLogeado.empty()) {
                    cout << "No estas logueado.\n";
                } else {
                    comando = "LOGOUT";
                    enviarComando(comando);
                    cout << recibirRespuesta() << endl;
                    usuarioLogeado = "";
                }
                break;
            }
            case 9: {
                conectado = false;
                break;
            }
            default:
                cout << "Opcion no valida.\n";
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
