#ifndef LISTAS_H
#define LISTAS_H //sirve evita errores si el archivo se incluye varias veces

#include <stdbool.h> //debe ir aqui por el uso del bool

typedef struct PCB {
    int PID;
    char nombre_proceso[50];
    int PC;
    char IR[64];
    int EAX,EBX,ECX,EDX;
    struct PCB *sig;
} PCB;

PCB *crear_nodo(int pid, char nombre_proceso[], int PC, char IR[], int EAX, int EBX, int ECX, int EDX);

void insertar(PCB *lista, PCB *nuevo);

void imprimir(PCB *lista, int numLista, int *numLinea);

PCB *sacarFrente(PCB *lista);

PCB *buscar_sacar(PCB *lista, int num_PID, bool condicion);

#endif