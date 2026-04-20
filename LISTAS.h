#ifndef LISTAS_H
#define LISTAS_H //sirve evita errores si el archivo se incluye varias veces

#include <stdbool.h> //debe ir aqui por el uso del bool

typedef struct PCB {
    int PID, GID;
    char nombre_proceso[50];
    int PC;
    char IR[64];
    int EAX,EBX,ECX,EDX;
    int CPU, GCPU, P; // P es Prioridad.
    struct PCB *sig;
} PCB;

PCB *crear_nodo(int pid, int gid, char nombre_proceso[], int PC, char IR[]);

void insertar(PCB *lista, PCB *nuevo);

void imprimir(PCB *lista, int numLista, int *numLinea);

PCB *sacarFrente(PCB *lista);

PCB *buscar_sacar(PCB *lista, int num_PID, bool condicion);

#endif