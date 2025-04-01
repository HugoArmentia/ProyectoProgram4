#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "citas.h"
#include "calendario.h"

#define HORAS_DISPONIBLES 24  
#define MAX_MESES 6

const char *horas[] = {"08:00", "08:30", "09:00", "09:30", "10:00", "10:30", "11:00", "11:30",
                      "12:00", "12:30", "13:00", "13:30", "14:00", "14:30", "15:00", "15:30",
                      "16:00", "16:30", "17:00", "17:30", "18:00", "18:30", "19:00", "19:30"};

void mostrarHorasDisponibles(int dia, int mes, int anio) {
    printf("\nHoras disponibles para %d-%d-%d:\n", dia, mes, anio);

    for (int i = 0; i < HORAS_DISPONIBLES; i++) {
        int disponible = 1;

        for (int j = 0; j < totalCitas; j++) {
            if (citas[j].dia == dia && citas[j].mes == mes && citas[j].anio == anio && strcmp(citas[j].fecha, horas[i]) == 0 && strcmp(citas[j].estado, "Programada") == 0) {
                disponible = 0;
                break;
            }
        }

        if (disponible) printf("%s\n", horas[i]);
        else printf("%s (Reservado)\n", horas[i]);
    }
}

void mostrarCalendarioMensual(int mes, int anio) {
    printf("\nCalendario para %d-%d\n", mes, anio);
    printf("  Lu Ma Mi Ju Vi Sa Do\n");

    struct tm t = {0};
    t.tm_mday = 1;
    t.tm_mon = mes - 1;
    t.tm_year = anio - 1900;
    mktime(&t);

    int diaInicio = t.tm_wday == 0 ? 7 : t.tm_wday;
    int diasEnMes = (mes == 2) ? (anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0) ? 29 : 28) :
                     (mes == 4 || mes == 6 || mes == 9 || mes == 11 ? 30 : 31);

    for (int i = 1; i < diaInicio; i++) printf("   ");

    for (int dia = 1; dia <= diasEnMes; dia++) {
        int reservado = 0;

        for (int j = 0; j < totalCitas; j++) {
            if (citas[j].dia == dia && citas[j].mes == mes && citas[j].anio == anio && strcmp(citas[j].estado, "Programada") == 0) {
                reservado = 1;
                break;
            }
        }

        if (reservado)
            printf("[%2d]", dia);  
        else
            printf(" %2d ", dia);  

        if ((dia + diaInicio - 1) % 7 == 0 || dia == diasEnMes) printf("\n");
    }
}

void reservarCitaDesdeCalendario(int paciente_id) {
    int dia, mes, anio, indiceHora;
    
    // Mostrar el calendario de los próximos meses
    mostrarCalendariosFuturos();

    printf("Ingrese el mes que desea reservar (1 - 6): ");
    scanf("%d", &mes);
    getchar();

    if (mes < 1 || mes > 6) {
        printf("Mes inválido.\n");
        return;
    }

    // Mostrar el calendario para ese mes
    time_t t = time(NULL);  // Obtener el tiempo actual
    struct tm *fechaActual = localtime(&t);  // Convertir a estructura tm

    anio = fechaActual->tm_year + 1900;  // Obtener el año actual
    int mesActual = fechaActual->tm_mon + 1;  // Obtener el mes actual (1-12)

    mes = mesActual + mes - 1;  // Ajustamos el mes según el valor ingresado

    if (mes > 12) {  // Si el mes supera diciembre, ajustamos el año
        mes -= 12;
        anio++;
    }

    printf("Ingrese el día que desea reservar: ");
    scanf("%d", &dia);
    getchar();

    // Mostrar las horas disponibles para ese día
    mostrarHorasDisponibles(dia, mes, anio);

    printf("Seleccione un índice de hora (0 - 23): ");
    scanf("%d", &indiceHora);
    getchar();

    if (indiceHora < 0 || indiceHora >= HORAS_DISPONIBLES) {
        printf("Índice de hora inválido.\n");
        return;
    }

    // Crear una nueva cita
    Cita nuevaCita;
    nuevaCita.id = totalCitas + 1;
    nuevaCita.paciente_id = paciente_id;
    nuevaCita.dia = dia;
    nuevaCita.mes = mes;
    nuevaCita.anio = anio;
    strcpy(nuevaCita.fecha, horas[indiceHora]);  // Hora de la cita

    printf("Cita reservada para: %d-%d-%d %s\n", dia, mes, anio, horas[indiceHora]);

    // Pedir el motivo de la cita
    printf("Ingrese el motivo de la cita: ");
    fgets(nuevaCita.motivo, sizeof(nuevaCita.motivo), stdin);
    nuevaCita.motivo[strcspn(nuevaCita.motivo, "\n")] = '\0';  // Limpiar el salto de línea

    strcpy(nuevaCita.estado, "Programada");

    citas[totalCitas] = nuevaCita;
    totalCitas++;

    guardarCitas();

    printf(" Cita reservada con éxito para la fecha: %d-%d-%d %s\n", dia, mes, anio, horas[indiceHora]);
}


void mostrarCalendariosFuturos() {
    time_t t = time(NULL);
    struct tm *fechaActual = localtime(&t);

    int anio = fechaActual->tm_year + 1900;
    int mes = fechaActual->tm_mon + 1;

    // Mostrar los calendarios de los próximos 6 meses
    for (int i = 0; i < MAX_MESES; i++) {
        mostrarCalendarioMensual(mes, anio);  // Llamar a la función que muestra el calendario de un mes específico
        mes++;  // Pasar al siguiente mes
        if (mes > 12) {  // Si el mes supera diciembre, pasar al siguiente año
            mes = 1;
            anio++;
        }
    }
}