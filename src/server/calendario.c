#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "citas.h"
#include "calendario.h"
#include "horas.h"
#include <sqlite3.h>
#include "database.h"

extern const char *horas[];

void mostrarHorasDisponibles(int dia, int mes, int anio) {
    printf("\nHoras disponibles para %d-%d-%d:\n", dia, mes, anio);

    const char *sql = "SELECT fecha FROM citas WHERE dia = ? AND mes = ? AND anio = ? AND estado = 'Programada';";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error al preparar consulta de horas: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, dia);
    sqlite3_bind_int(stmt, 2, mes);
    int usadas[HORAS_DISPONIBLES] = {0};

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *horaReservada = (const char *)sqlite3_column_text(stmt, 0);
        for (int i = 0; i < HORAS_DISPONIBLES; i++) {
            if (strcmp(horas[i], horaReservada) == 0) {
                usadas[i] = 1;
                break;
            }
        }
    }

    sqlite3_finalize(stmt);

    for (int i = 0; i < HORAS_DISPONIBLES; i++) {
        if (usadas[i])
            printf("%s (Reservado)\n", horas[i]);
        else
            printf("%s\n", horas[i]);
    }
}

void mostrarCalendarioMensual(int mes, int anio) {
    printf("\nCalendario para %d-%d\n", mes, anio);
    printf("  Lu Ma Mi Ju Vi Sa Do\n");

    time_t t = time(NULL);
    struct tm *fechaActual = localtime(&t);
    int diaHoy = fechaActual->tm_mday;
    int mesActual = fechaActual->tm_mon + 1;
    int anioActual = fechaActual->tm_year + 1900;

    struct tm t_ini = {0};
    t_ini.tm_mday = 1;
    t_ini.tm_mon = mes - 1;
    t_ini.tm_year = anio - 1900;
    mktime(&t_ini);

    int diaInicio = t_ini.tm_wday == 0 ? 7 : t_ini.tm_wday;
    int diasEnMes = (mes == 2) ? (anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0) ? 29 : 28) :
                     (mes == 4 || mes == 6 || mes == 9 || mes == 11 ? 30 : 31);

    for (int i = 1; i < diaInicio; i++) printf("   ");

    for (int dia = 1; dia <= diasEnMes; dia++) {
        if (anio == anioActual && mes == mesActual && dia < diaHoy) {
            printf(" -- ");
            continue;
        }

        int reservado = 0;
        const char *sql = "SELECT COUNT(*) FROM citas WHERE dia = ? AND mes = ? AND anio = ? AND estado = 'Programada';";
        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, dia);
            sqlite3_bind_int(stmt, 2, mes);
            sqlite3_bind_int(stmt, 3, anio);

            if (sqlite3_step(stmt) == SQLITE_ROW) {
                if (sqlite3_column_int(stmt, 0) > 0) reservado = 1;
            }

            sqlite3_finalize(stmt);
        }

        if (reservado)
            printf("[%2d]", dia);
        else
            printf(" %2d ", dia);

        if ((dia + diaInicio - 1) % 7 == 0 || dia == diasEnMes) printf("\n");
    }
}


void reservarCitaDesdeCalendario(int paciente_id) {
    int dia, mes, anio, indiceHora, medico_id;

    mostrarCalendariosFuturos();

    printf("Ingrese el mes que desea reservar : ");
    scanf("%d", &mes);
    getchar();

    if (mes < 1 || mes > 6) {
        printf("Mes inválido.\n");
        return;
    }

    time_t t = time(NULL);
    struct tm *fechaActual = localtime(&t);
    int diaHoy = fechaActual->tm_mday;
    int mesActual = fechaActual->tm_mon + 1;
    int anioActual = fechaActual->tm_year + 1900;

    anio = anioActual; 

    printf("Ingrese el día que desea reservar: ");
    scanf("%d", &dia);
    getchar();

    if ((anio < anioActual) ||
        (anio == anioActual && mes < mesActual) ||
        (anio == anioActual && mes == mesActual && dia < diaHoy)) {
        printf("No puedes reservar una cita en una fecha anterior a hoy.\n");
        return;
    }


    mostrarHorasDisponibles(dia, mes, anio);

    printf("Seleccione un índice de hora (0 - 23): ");
    scanf("%d", &indiceHora);
    getchar();

    if (indiceHora < 0 || indiceHora >= HORAS_DISPONIBLES) {
        printf("Índice de hora inválido.\n");
        return;
    }

    printf("Ingrese el ID del médico con el que desea reservar la cita: ");
    scanf("%d", &medico_id);
    getchar();

    char motivo[100];
    printf("Ingrese el motivo de la cita: ");
    fgets(motivo, sizeof(motivo), stdin);
    motivo[strcspn(motivo, "\n")] = 0;

    const char *checkSql = "SELECT COUNT(*) FROM citas "
                        "WHERE dia = ? AND mes = ? AND anio = ? AND fecha = ? AND medico_id = ? AND estado = 'Programada';";

    sqlite3_stmt *checkStmt;
    if (sqlite3_prepare_v2(db, checkSql, -1, &checkStmt, NULL) != SQLITE_OK) {
        printf("Error al preparar la consulta de comprobación: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(checkStmt, 1, dia);
    sqlite3_bind_int(checkStmt, 2, mes);
    sqlite3_bind_int(checkStmt, 3, anio);
    sqlite3_bind_text(checkStmt, 4, horas[indiceHora], -1, SQLITE_STATIC);
    sqlite3_bind_int(checkStmt, 5, medico_id);

    int ocupado = 0;
    if (sqlite3_step(checkStmt) == SQLITE_ROW) {
        ocupado = sqlite3_column_int(checkStmt, 0);
    }
    sqlite3_finalize(checkStmt);

    if (ocupado > 0) {
        printf("⚠️ El médico ya tiene una cita en esa hora. Elija otra hora.\n");
        return;
    }

    const char *sql = "INSERT INTO citas (paciente_id, medico_id, fecha, estado, motivo, dia, mes, anio, fecha_modificacion) "
                      "VALUES (?, ?, ?, 'Programada', ?, ?, ?, ?, datetime('now'));";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error al preparar la inserción de cita: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, paciente_id);
    sqlite3_bind_int(stmt, 2, medico_id);
    sqlite3_bind_text(stmt, 3, horas[indiceHora], -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, motivo, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, dia);
    sqlite3_bind_int(stmt, 6, mes);
    sqlite3_bind_int(stmt, 7, anio);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Cita reservada con éxito para la fecha: %d-%d-%d %s\n", dia, mes, anio, horas[indiceHora]);
    } else {
        printf("Error al reservar cita: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}


void mostrarCalendariosFuturos() {
    time_t t = time(NULL);
    struct tm *fechaActual = localtime(&t);

    int anio = fechaActual->tm_year + 1900;
    int mes = fechaActual->tm_mon + 1;

    for (int i = 0; i < MAX_MESES; i++) {
        mostrarCalendarioMensual(mes, anio);
        mes++;
        if (mes > 12) {
            mes = 1;
            anio++;
        }
    }
}